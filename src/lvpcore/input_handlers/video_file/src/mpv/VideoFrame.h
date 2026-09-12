#pragma once

#include <cstdint>
#include <vector>

class VideoFrame
{
public:
    VideoFrame() = default;
    ~VideoFrame() = default;

    VideoFrame(const VideoFrame&) = default;
    VideoFrame& operator=(const VideoFrame&) = default;

    VideoFrame(VideoFrame&&) noexcept = default;
    VideoFrame& operator=(VideoFrame&&) noexcept = default;

    void Resize(uint32_t width, uint32_t height);

    uint8_t* Data();
    const uint8_t* Data() const;

    size_t DataSize() const;

public:
    // Timestamp and duration in microseconds.
    int64_t timestamp = 0;
    int64_t duration = 0;

    uint32_t width = 0;
    uint32_t height = 0;

    uint32_t stride = 0;

    // Initial pixel format is RGBA8.
    std::vector<uint8_t> data;
};