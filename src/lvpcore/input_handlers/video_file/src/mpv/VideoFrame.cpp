#include "VideoFrame.h"

void VideoFrame::Resize(uint32_t newWidth, uint32_t newHeight)
{
    width = newWidth;
    height = newHeight;

    // RGBA8
    stride = width * 4;

    data.resize(static_cast<size_t>(stride) * height);
}

uint8_t* VideoFrame::Data()
{
    return data.data();
}

const uint8_t* VideoFrame::Data() const
{
    return data.data();
}

size_t VideoFrame::DataSize() const
{
    return data.size();
}