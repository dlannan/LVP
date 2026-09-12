#include "stateVideoFileStream.h"

#include <string>
#include <iostream>

// VideoFileStream
//  What it is: From an input video file, control the input for streaming frames
//              including frame rate, resolution, color format, source and target
//  Command params: <name> <port> <target> 
//       name - the name of this state to be used
//       port - the port this process will use when atlas registers it
//       target - the target for any video frames or stream to send to. A "null" target doesnt send.
//
//  The video file stream is the base file input method for fetching video frames from a file.
//  The primary library used is libmpv for convienience.

namespace
{
    constexpr const char* RegisterCommand = "register";
    constexpr const char* RegisterResponse = "registered";
    constexpr const char* VFS_Open_Command = "video_file_open";
    constexpr const char* VFS_Play_Command = "video_file_play";
    constexpr const char* VFS_Close_Command = "video_file_close";
}

stateVideoFileStream::stateVideoFileStream(std::string name, std::string target, state_manager *smanager)
    : m_input(nullptr)
    , m_output(nullptr)
    , m_running(false)
    , m_name(name)
    , m_target(target)
    , m_smanager(smanager)
    , m_buffer(30)
{

}
stateVideoFileStream::~stateVideoFileStream()
{
    Finish();
}

bool stateVideoFileStream::Init(state_stream& input, state_stream& output)  
{
    m_input = &input;
    m_output = &output;

    std::cout << m_name << ": Init\n";
    return true;
}

bool stateVideoFileStream::Begin()
{
    if (m_input == nullptr || m_output == nullptr)
    {
        return false;
    }

    std::cout << m_name << ": Begin\n";
    m_running = true;
    m_lasttime = dt;
    return true;
}

void stateVideoFileStream::VideoSendStream()
{
    state_packet packet;

    m_uid = m_smanager->GetState("video_file_register")->getUid();
    packet.envelope.uid = m_uid;
    std::cout << "m_uid: " << m_uid << "\n";
    packet.envelope.owner = m_name.c_str();
    packet.envelope.target = m_target.c_str();
    packet.envelope.memoryScope = "local";
    packet.metadata.push_back({ "command", VFS_Open_Command });

    if (!m_output->write(packet))
    {
        std::cout << m_name << ": failed to queue forward\n";
        return;
    }

    std::cout << m_name << ": forward queued\n";
}

void stateVideoFileStream::VFSOpenCommand()
{
    state_packet packet;
    m_input->read(packet);

    m_source.SetFrameBuffer(&m_buffer);
    m_source.Open("F:\\dev\\cdev\\LVP\\data\\test_video\\sample-1mb.mp4");

    if (packet.envelope.target.empty())
    {
        std::cout << m_name << ": unknown forward target\n";
    }

    // Consume the packet
    std::cout
        << m_name << ": forward with atlas uid="
        << m_uid
        << '\n';
}

void stateVideoFileStream::VFSPlayCommand()
{
    m_source.Play();
}

void stateVideoFileStream::VFSCloseCommand()
{
}

void stateVideoFileStream::PreUpdate()
{

}

void stateVideoFileStream::Update(int px, int py, int buttons)
{
    if (!m_running || m_input == nullptr)
        return;

    int processed = 0;

    if (m_source.IsOpen())
    {
        m_source.ProcessEvents();

        VideoFrame frame;

        if (m_source.CaptureFrame(frame))
            m_buffer.Push(std::move(frame));
        // ...
    }    

    state_packet packet;

    if(m_input->check(packet))
    {
        ++processed;
        m_command = VFS_CMD_NONE;

        for (const auto& metadata : packet.metadata)
        {
            if (metadata.key == "command")
            {
                // Recieved a video_file command from another process, do it.
                if(metadata.value == VFS_Open_Command)
                {
                    m_command = VFS_CMD_OPEN;
                    break;
                }
                else if(metadata.value == VFS_Play_Command)
                {
                    m_command = VFS_CMD_PLAY;
                    break;
                }
                else if(metadata.value == VFS_Close_Command)
                {
                    m_command = VFS_CMD_CLOSE;
                    break;
                }
            }    
        }

        // Not a valid command packet, throw it away.
        if (m_command == VFS_CMD_NONE)
        {
            //std::cout << m_name << ": ignoring packet\n";
            m_input->read(packet);
            return;
        }

        switch(m_command) {
            case VFS_CMD_OPEN:
                VFSOpenCommand();
                break;
            case VFS_CMD_PLAY:
                VFSPlayCommand();
                break;
            case VFS_CMD_CLOSE:
                VFSCloseCommand();
                break;
        }
    }

    m_processed += processed;
    return;
}

void stateVideoFileStream::Render()
{

}

void stateVideoFileStream::PostUpdate()
{

}

void stateVideoFileStream::Finish()
{
    if (!m_running)
    {
        return;
    }

    std::cout << m_name << ": Finish\n";
    m_running = false;
}