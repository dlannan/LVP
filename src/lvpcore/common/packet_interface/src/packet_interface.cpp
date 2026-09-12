#include "../include/packet_interface.h"

#include <uv.h>

#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace
{
constexpr std::uint32_t ProtocolMagic = 0x4D4F4431; // MOD1
constexpr std::uint16_t ProtocolVersion = 1;

constexpr const char* AtlasTarget = "atlas";

struct wire_header
{
    std::uint32_t magic;
    std::uint16_t version;
    std::uint16_t reserved;

    std::uint64_t uid;
    std::uint64_t timestamp;

    std::uint32_t ownerSize;
    std::uint32_t targetSize;
    std::uint32_t memoryScopeSize;

    std::uint32_t metadataCount;
    std::uint32_t payloadSize;
};

void append(std::vector<std::uint8_t>& buffer, const void* data, std::size_t size)
{
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    buffer.insert( buffer.end(), bytes, bytes + size);
}

bool readBytes(const std::uint8_t*& cursor, std::size_t& remaining, void* destination, std::size_t size)
{
    if (remaining < size)
    {
        return false;
    }

    std::memcpy(destination, cursor, size);

    cursor += size;
    remaining -= size;

    return true;
}

void appendString(std::vector<std::uint8_t>& buffer, const std::string& value)
{
    append(buffer, value.data(), value.size());
}

bool readString(const std::uint8_t*& cursor, std::size_t& remaining, std::uint32_t size, std::string& value)
{
    if (remaining < size)
    {
        return false;
    }
    value.assign(reinterpret_cast<const char*>(cursor), size);

    cursor += size;
    remaining -= size;

    return true;
}

bool serialize( const state_packet& packet, std::vector<std::uint8_t>& buffer) 
{
    wire_header header{};

    header.magic = ProtocolMagic;
    header.version = ProtocolVersion;

    header.uid = packet.envelope.uid;
    header.timestamp = packet.envelope.timestamp;

    header.ownerSize = static_cast<std::uint32_t>(packet.envelope.owner.size());
    header.targetSize = static_cast<std::uint32_t>(packet.envelope.target.size());
    header.memoryScopeSize = static_cast<std::uint32_t>(packet.envelope.memoryScope.size());
    header.metadataCount = static_cast<std::uint32_t>(packet.metadata.size());
    header.payloadSize = static_cast<std::uint32_t>(packet.payload.size());

    buffer.clear();

    append(buffer, &header, sizeof(header));
    appendString(buffer, packet.envelope.owner);
    appendString(buffer, packet.envelope.target);
    appendString(buffer, packet.envelope.memoryScope);

    for (const auto& metadata : packet.metadata)
    {
        const std::uint32_t keySize = static_cast<std::uint32_t>(metadata.key.size());
        const std::uint32_t valueSize = static_cast<std::uint32_t>(metadata.value.size());

        append(buffer, &keySize, sizeof(keySize));
        append(buffer, &valueSize, sizeof(valueSize));
        appendString( buffer, metadata.key);
        appendString( buffer, metadata.value);
    }

    if (!packet.payload.empty())
    {
        append(buffer, packet.payload.data(), packet.payload.size());
    }
    return true;
}

bool deserialize(const std::uint8_t* data, std::size_t size, state_packet& packet)
{
    const std::uint8_t* cursor = data;
    std::size_t remaining = size;

    wire_header header{};

    if (!readBytes(cursor, remaining, &header, sizeof(header)))
    {
        return false;
    }

    if (header.magic != ProtocolMagic || header.version != ProtocolVersion)
    {
        return false;
    }

    packet = {};

    packet.envelope.uid = header.uid;
    packet.envelope.timestamp = header.timestamp;

    if (!readString(cursor, remaining, header.ownerSize, packet.envelope.owner))
    {
        return false;
    }

    if (!readString(cursor, remaining, header.targetSize, packet.envelope.target))
    {
        return false;
    }

    if (!readString(cursor, remaining, header.memoryScopeSize, packet.envelope.memoryScope))
    {
        return false;
    }

    for (std::uint32_t i = 0; i < header.metadataCount; ++i)
    {
        std::uint32_t keySize = 0;
        std::uint32_t valueSize = 0;

        if (!readBytes(cursor, remaining,&keySize, sizeof(keySize)))
        {
            return false;
        }

        if (!readBytes(cursor, remaining, &valueSize, sizeof(valueSize)))
        {
            return false;
        }

        state_metadata metadata;

        if (!readString(cursor, remaining, keySize, metadata.key))
        {
            return false;
        }

        if (!readString(cursor, remaining, valueSize, metadata.value))
        {
            return false;
        }

        packet.metadata.push_back(std::move(metadata));
    }

    if (remaining != header.payloadSize)
    {
        return false;
    }

    packet.payload.assign(cursor, cursor + remaining);
    return true;
}

struct send_context
{
    uv_udp_send_t request;
    std::vector<std::uint8_t> data;
};

}

class packet_interface::stream final : public state_stream
{
    public:
    bool available() const override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_packets.empty();
    }

    bool check(state_packet& packet) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_packets.empty())
            return false;
        packet = m_packets.front();
        return true;
    }

    bool read(state_packet& packet) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_packets.empty())
            return false;
        packet = std::move(m_packets.front());
        m_packets.pop();
        return true;
    }

    bool write(const state_packet& packet) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_packets.push(packet);
        return true;
    }

    bool pop(state_packet& packet)
    {
        return read(packet);
    }

    private:
    mutable std::mutex m_mutex;
    std::queue<state_packet> m_packets;
};

struct packet_interface::transport
{
    struct received_packet
    {
        state_packet packet;
        sockaddr_in source{};
    };

    uv_loop_t* loop = nullptr;

    uv_udp_t socket{};

    /*
    * For a state interface this is the endpoint
    * of its Atlas.
    */
    sockaddr_in atlasEndpoint{};
    bool atlasKnown = true;

    /*
    * For an Atlas interface this maps logical
    * state names to transport endpoints.
    */
    std::unordered_map<std::string, sockaddr_in> endpoints;

    /*
    * Received packets are retained here together
    * with their UDP source endpoint.
    */
    std::queue<received_packet> received;

    bool initialised = false;

    void setAtlas(sockaddr_in addr  = {}) { 
        if(addr.sin_port == 0 && addr.sin_family == 0) {
            uv_ip4_addr("127.0.0.1", 42000, &atlasEndpoint); 
        }
        else {
            atlasEndpoint = addr;
        }
    }

    static void allocBuffer( uv_handle_t*, std::size_t suggestedSize, uv_buf_t* buffer)
    {
        buffer->base = new char[suggestedSize];
        buffer->len = static_cast<unsigned int>(suggestedSize);
    }

    static void onReceive(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buffer, const sockaddr* address, unsigned)
    {
        auto* transport = static_cast<packet_interface::transport*>(handle->data);

        if (nread > 0 && address != nullptr && address->sa_family == AF_INET)
        {
            state_packet packet;

            if (deserialize(reinterpret_cast<const std::uint8_t*>(buffer->base), static_cast<std::size_t>(nread), packet))
            {
                received_packet received;
                received.packet = std::move(packet);
                received.source = *reinterpret_cast<const sockaddr_in*>(address);

                transport->received.push(std::move(received));
            }
            else
            {
                std::cerr << "packet_interface: invalid UDP packet\n";
            }
        }

        delete[] buffer->base;
    }

};

packet_interface::packet_interface(const packet_interface_config& config)
    : m_config(config)
    , m_readStream(new stream())
    , m_writeStream(new stream())
    , m_transport(new transport())
    , m_mutex()
    , m_cv()
{
    if(config.domain == packet_interface_atlas_domain::local)
        m_transport->setAtlas();
    else if(config.domain == packet_interface_atlas_domain::dns)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        addrinfo* result = nullptr;

        int ok = getaddrinfo(config.atlas.c_str(),nullptr, &hints, &result);
        if(ok) { 
            m_transport->setAtlas(*reinterpret_cast<sockaddr_in*>(result->ai_addr)); 
            uv_freeaddrinfo(result);
        }
    }
}

packet_interface::~packet_interface()
{
    Finish();

    delete m_transport;
    delete m_readStream;
    delete m_writeStream;
}

bool packet_interface::Init()
{
    std::cout
    << "packet_interface initialising on port "
    << m_config.port
    << '\n';

    if (m_config.atlas.empty())
    {
        std::cerr << "packet_interface: Atlas identity is empty\n";
        return false;
    }

    m_transport->loop = uv_default_loop();

    if (uv_udp_init(m_transport->loop, &m_transport->socket) != 0)
    {
        std::cerr << "packet_interface: uv_udp_init failed\n";
        return false;
    }

    m_transport->socket.data = m_transport;
    sockaddr_in localAddress{};

    if (uv_ip4_addr("0.0.0.0", m_config.port,  &localAddress) != 0)
    {
        return false;
    }

    if (uv_udp_bind(&m_transport->socket, reinterpret_cast<const sockaddr*>(&localAddress),
        UV_UDP_REUSEADDR) != 0)
    {
        std::cerr << "packet_interface: uv_udp_bind failed\n";
        return false;
    }

    m_transport->initialised = true;

    if (uv_udp_recv_start(&m_transport->socket, transport::allocBuffer, transport::onReceive) != 0)
    {
        std::cerr << "packet_interface: uv_udp_recv_start failed\n";
        return false;
    }

    return true;
}

int packet_interface::Update()
{
    if (!m_transport->initialised)
    {
        return -1;
    }

    int processed = 0;

    /*
    * Dispatch pending UDP receive events.
    */
    uv_run(m_transport->loop, UV_RUN_NOWAIT);

    /*
    * Move received packets into the state
    * input stream.
    *
    * Endpoint handling is deliberately kept
    * inside packet_interface.
    */
    while (!m_transport->received.empty())
    {
        auto received = std::move( m_transport->received.front());
        m_transport->received.pop();

        /*
        * A state receives packets from Atlas.
        *
        * The first valid packet from the configured
        * Atlas establishes its transport endpoint.
        */
        if (m_config.role == packet_interface_role::state)
        {
            if (!m_transport->atlasKnown)
            {
                m_transport->atlasEndpoint = received.source;
                m_transport->atlasKnown = true;

                std::cout << "packet_interface: Atlas discovered\n";
            }

            m_readStream->write(received.packet);
            ++processed;

            continue;
        }

        /*
        * Atlas learns the transport endpoint of the
        * state identified by the packet owner.
        */
        if (m_config.role == packet_interface_role::atlas)
        {
            if (!received.packet.envelope.owner.empty())
            {
                auto is_mapped = m_transport->endpoints.find(received.packet.envelope.owner);
                if(is_mapped == m_transport->endpoints.end()) {
                    std::cout << "Mapping: " << received.packet.envelope.owner << "\n";
                    m_transport->endpoints[received.packet.envelope.owner] = received.source;
                }
            }

            m_readStream->write(received.packet);
            ++processed;
        }
    }

    /*
    * Process outgoing packets.
    */
    state_packet packet;

    while (m_writeStream->pop(packet))
    {
        sockaddr_in destination{};
        bool haveDestination = false;

        if (m_config.role == packet_interface_role::state)
        {
            // Atlas is always known. Default is 42000 (this will be configurable for a state)
            destination = m_transport->atlasEndpoint;
            haveDestination = true;
        }
        else if (m_config.role == packet_interface_role::atlas)
        {
            /*
            * Atlas routes by logical target.
            */
            const auto iterator = m_transport->endpoints.find(packet.envelope.target);

            if (iterator == m_transport->endpoints.end())
            {
                std::cerr
                    << "packet_interface: no endpoint for target "
                    << packet.envelope.target
                    << '\n';

                    for (const auto& [key, value] : m_transport->endpoints)
                    {
                        std::cout << key << '\n';
                    }

                continue;
            }

            destination = iterator->second;
            haveDestination = true;
        }

        if (!haveDestination)
        {
            continue;
        }

        std::vector<std::uint8_t> data;

        if (!serialize(packet, data))
        {
            continue;
        }

        auto* request = new send_context();

        request->data = std::move(data);

        uv_buf_t buffer = uv_buf_init(
                reinterpret_cast<char*>(
                    request->data.data()),
                static_cast<unsigned int>(
                    request->data.size()));

        const int result = uv_udp_send(&request->request, &m_transport->socket, &buffer, 1,
                reinterpret_cast<const sockaddr*>(&destination),
                [](uv_udp_send_t* request, int status)
                {
                    auto* context = reinterpret_cast<send_context*>(request);
                    if (status < 0)
                    {
                        std::cerr
                            << "packet_interface: UDP send failed: "
                            << uv_strerror(status)
                            << '\n';
                    }

                    delete context;
                });

        if (result != 0)
        {
            delete request;
            std::cerr
                << "packet_interface: uv_udp_send failed: "
                << uv_strerror(result)
                << '\n';

            continue;
        }

        ++processed;
    }

    /*
    * Dispatch callbacks generated by sends.
    */
    uv_run( m_transport->loop, UV_RUN_NOWAIT);
    return processed;
}

void packet_interface::Wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
        uv_run(m_transport->loop, UV_RUN_NOWAIT);
        return !m_readStream->available();
    });
}

void packet_interface::Finish()
{
    if (m_transport == nullptr || !m_transport->initialised)
    {
        return;
    }

    std::cout << "packet_interface finishing...\n";

    uv_close(reinterpret_cast<uv_handle_t*>(&m_transport->socket), nullptr);
    uv_run(m_transport->loop, UV_RUN_NOWAIT);
    m_transport->initialised = false;

}

state_stream& packet_interface::readStream()
{
    return *m_readStream;
}

state_stream& packet_interface::writeStream()
{
    return *m_writeStream;
}