#include "MpvVideoSource.h"

#include <mpv/client.h>
#include <mpv/render.h>

#include <cstring>

MpvVideoSource::MpvVideoSource()
{
}

MpvVideoSource::~MpvVideoSource()
{
    Close();
}

bool MpvVideoSource::Open(const char* filename)
{
    Close();

    mpv = mpv_create();

    if (!mpv)
        return false;

    // Don't create a normal video output window.
    if (mpv_set_option_string(mpv, "vo", "libmpv") < 0)
    {
        Close();
        return false;
    }

    // Start paused. The caller explicitly starts playback.
    if (mpv_set_option_string(mpv, "pause", "yes") < 0)
    {
        Close();
        return false;
    }

    // We want video output but no UI.
    if (mpv_set_option_string(mpv, "terminal", "no") < 0)
    {
        Close();
        return false;
    }

    if (mpv_initialize(mpv) < 0)
    {
        Close();
        return false;
    }

    if (!CreateRenderContext())
    {
        Close();
        return false;
    }

    const char* command[] =
    {
        "loadfile",
        filename,
        nullptr
    };

    if (mpv_command(mpv, command) < 0)
    {
        Close();
        return false;
    }

    fileOpen = true;

    return true;
}

bool MpvVideoSource::CreateRenderContext()
{
    mpv_render_param param = {};

    param.type = MPV_RENDER_PARAM_API_TYPE;
    param.data = (void*)MPV_RENDER_API_TYPE_SW;

    const int result =
        mpv_render_context_create(
            &render,
            mpv,
            &param);

    return result >= 0;
}

void MpvVideoSource::Close()
{
    if (render)
    {
        mpv_render_context_free(render);
        render = nullptr;
    }

    if (mpv)
    {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }

    fileOpen = false;

    width = 0;
    height = 0;
}

bool MpvVideoSource::Play()
{
    if (!mpv)
    return false;

    int value = 0;

    return mpv_set_property(
        mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &value) >= 0;
}

bool MpvVideoSource::Pause()
{
    if (!mpv)
    return false;

    int value = 1;

    return mpv_set_property(
        mpv,
        "pause",
        MPV_FORMAT_FLAG,
        &value) >= 0;
}

bool MpvVideoSource::IsOpen() const
{
    return fileOpen;
}

bool MpvVideoSource::IsPaused() const
{
    if (!mpv)
    return true;

    int value = 1;

    if (mpv_get_property(
            mpv,
            "pause",
            MPV_FORMAT_FLAG,
            &value) < 0)
    {
        return true;
    }

    return value != 0;
}

bool MpvVideoSource::ProcessEvents()
{
    if (!mpv)
        return false;

    bool ok = true;

    while (true)
    {
        mpv_event* event =
            mpv_wait_event(mpv, 0.0);

        if (!event)
            break;

        switch (event->event_id)
        {
        case MPV_EVENT_VIDEO_RECONFIG:
        {
            // Video dimensions may have changed.
            int64_t w = 0;
            int64_t h = 0;

            if (mpv_get_property(
                    mpv,
                    "video-out-params/w",
                    MPV_FORMAT_INT64,
                    &w) >= 0)
            {
                width = static_cast<int>(w);
            }

            if (mpv_get_property(
                    mpv,
                    "video-out-params/h",
                    MPV_FORMAT_INT64,
                    &h) >= 0)
            {
                height = static_cast<int>(h);
            }

            break;
        }

        case MPV_EVENT_END_FILE:
            fileOpen = false;
            break;

        case MPV_EVENT_SHUTDOWN:
            fileOpen = false;
            ok = false;
            break;

        default:
            break;
        }
    }

    return ok;
}

bool MpvVideoSource::CaptureFrame(VideoFrame& frame)
{
    if (!mpv || !render)
        return false;

    if (width <= 0 || height <= 0)
        return false;

    frame.Resize(
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height));

    int size[2] =
    {
        width,
        height
    };

    char format[] = "rgba";

    size_t stride = frame.stride;

    mpv_render_param params[5] = {};

    params[0].type = MPV_RENDER_PARAM_SW_SIZE;
    params[0].data = size;

    params[1].type = MPV_RENDER_PARAM_SW_FORMAT;
    params[1].data = format;

    params[2].type = MPV_RENDER_PARAM_SW_STRIDE;
    params[2].data = &stride;

    params[3].type = MPV_RENDER_PARAM_SW_POINTER;
    params[3].data = frame.Data();

    params[4].type = MPV_RENDER_PARAM_INVALID;
    params[4].data = nullptr;

    const int result =
        mpv_render_context_render(
            render,
            params);

    if (result < 0)
        return false;

    frame.stride =
        static_cast<uint32_t>(stride);

    // Get current presentation timestamp.
    double time = 0.0;

    if (mpv_get_property(
            mpv,
            "time-pos",
            MPV_FORMAT_DOUBLE,
            &time) >= 0)
    {
        frame.timestamp =
            static_cast<int64_t>(time * 1000000.0);
    }

    return true;
}

int MpvVideoSource::Width() const
{
    return width;
}

int MpvVideoSource::Height() const
{
    return height;
}

double MpvVideoSource::CurrentTime() const
{
    if (!mpv)
    return 0.0;

    double value = 0.0;

    if (mpv_get_property(
            mpv,
            "time-pos",
            MPV_FORMAT_DOUBLE,
            &value) < 0)
    {
        return 0.0;
    }

    return value;
}

void MpvVideoSource::SetFrameBuffer(FrameRingBuffer* buffer)
{
    frameBuffer = buffer;
}