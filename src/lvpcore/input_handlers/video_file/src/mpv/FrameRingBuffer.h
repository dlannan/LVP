#pragma once

#include "VideoFrame.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class FrameRingBuffer
{
public:
    explicit FrameRingBuffer(size_t capacity);

    bool Push(VideoFrame&& frame);

    VideoFrame* GetLatest();
    const VideoFrame* GetLatest() const;

    VideoFrame* GetByTimestamp(int64_t timestamp);
    const VideoFrame* GetByTimestamp(int64_t timestamp) const;

    VideoFrame* GetByIndex(size_t index);
    const VideoFrame* GetByIndex(size_t index) const;

    size_t Size() const;
    size_t Capacity() const;

    void Clear();

private:
    std::vector<VideoFrame> frames;

    size_t writeIndex = 0;
    size_t count = 0;
};