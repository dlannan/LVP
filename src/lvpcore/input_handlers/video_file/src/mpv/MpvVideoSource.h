#pragma once

#include "FrameRingBuffer.h"

#include <cstdint>

struct mpv_handle;
struct mpv_render_context;

class MpvVideoSource
{
public:
    MpvVideoSource();
    ~MpvVideoSource();

    MpvVideoSource(const MpvVideoSource&) = delete;
    MpvVideoSource& operator=(const MpvVideoSource&) = delete;

    bool Open(const char* filename);
    void Close();

    bool Play();
    bool Pause();

    bool IsOpen() const;
    bool IsPaused() const;

    // Process pending libmpv events.
    // Returns false if the source has encountered a fatal error
    // or has been closed.
    bool ProcessEvents();

    // Render the current decoded video frame into the supplied
    // VideoFrame.
    //
    // This does not display anything. It copies the decoded/rendered
    // image into application-owned memory.
    bool CaptureFrame(VideoFrame& frame);

    int Width() const;
    int Height() const;

    double CurrentTime() const;

    void SetFrameBuffer(FrameRingBuffer* buffer);

private:
    bool CreateRenderContext();

    mpv_handle* mpv = nullptr;
    mpv_render_context* render = nullptr;

    FrameRingBuffer* frameBuffer = nullptr;

    int width = 0;
    int height = 0;

    bool fileOpen = false;
};