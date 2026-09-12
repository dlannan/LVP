#pragma once

#include "../../../common/state/include/state.h"
#include "../../../common/state/include/state_packet.h"
#include "../../../common/state/include/state_stream.h"
#include "../../../common/state/include/state_manager.h"

#include "../mpv/VideoFrame.h"
#include "../mpv/FrameRingBuffer.h"
#include "../mpv/MpvVideoSource.h"

#include <cstdint>
#include <unordered_map>

using state_uid = std::uint64_t;

enum VFS_Commands {
    VFS_CMD_NONE,
    VFS_CMD_OPEN,
    VFS_CMD_PLAY,
    VFS_CMD_CLOSE,
};

class stateVideoFileStream : public state
{
public:
    stateVideoFileStream(std::string name, std::string target, state_manager *smanager);
    ~stateVideoFileStream();

    stateVideoFileStream(const stateVideoFileStream&) = delete;
    stateVideoFileStream& operator=(const stateVideoFileStream&) = delete;

    bool Init(state_stream& input, state_stream& output) override;
    bool Begin() override;

    void PreUpdate() override;
    void Update(int px, int py, int buttons) override;
    void Render() override;
    void PostUpdate() override;

    void Finish() override;

    // Number of forwarded successes
    int m_processed = 0;
    std::string m_target;
    std::uint64_t m_ipuid = 0;

private:
    void VideoSendStream();
    void VFSOpenCommand();
    void VFSPlayCommand();
    void VFSCloseCommand();

    state_manager * m_smanager;

    FrameRingBuffer m_buffer;
    MpvVideoSource  m_source;

    VFS_Commands    m_command = VFS_CMD_NONE;

    double          m_lasttime = 0.0;
    double          m_current = 0.0;

    state_stream*   m_input;
    state_stream*   m_output;

    std::string     m_name;

    bool            m_running;
};