#include "FrameRingBuffer.h"

FrameRingBuffer::FrameRingBuffer(size_t capacity)
: frames(capacity)
{
}

bool FrameRingBuffer::Push(VideoFrame&& frame)
{
    if (frames.empty())
    return false;

    frames[writeIndex] = std::move(frame);

    writeIndex = (writeIndex + 1) % frames.size();

    if (count < frames.size())
        ++count;

    return true;
}

VideoFrame* FrameRingBuffer::GetLatest()
{
    if (count == 0)
    return nullptr;

    const size_t index =
        (writeIndex + frames.size() - 1) % frames.size();

    return &frames[index];
}

const VideoFrame* FrameRingBuffer::GetLatest() const
{
    if (count == 0)
    return nullptr;

    const size_t index =
        (writeIndex + frames.size() - 1) % frames.size();

    return &frames[index];
}

VideoFrame* FrameRingBuffer::GetByTimestamp(int64_t timestamp)
{
    if (count == 0)
    return nullptr;

    VideoFrame* best = nullptr;

    for (size_t i = 0; i < count; ++i)
    {
        const size_t index =
            (writeIndex + frames.size() - count + i) % frames.size();

        VideoFrame& frame = frames[index];

        if (frame.timestamp <= timestamp)
            best = &frame;
        else
            break;
    }

    return best;
}

const VideoFrame* FrameRingBuffer::GetByTimestamp( int64_t timestamp) const
{
    if (count == 0)
    return nullptr;

    const VideoFrame* best = nullptr;

    for (size_t i = 0; i < count; ++i)
    {
        const size_t index =
            (writeIndex + frames.size() - count + i) % frames.size();

        const VideoFrame& frame = frames[index];

        if (frame.timestamp <= timestamp)
            best = &frame;
        else
            break;
    }

    return best;
}

VideoFrame* FrameRingBuffer::GetByIndex(size_t index)
{
    if (index >= count)
    return nullptr;

    const size_t bufferIndex =
        (writeIndex + frames.size() - count + index)
        % frames.size();

    return &frames[bufferIndex];
}

const VideoFrame* FrameRingBuffer::GetByIndex(size_t index) const
{
    if (index >= count)
    return nullptr;

    const size_t bufferIndex =
        (writeIndex + frames.size() - count + index)
        % frames.size();

    return &frames[bufferIndex];
}

size_t FrameRingBuffer::Size() const
{
    return count;
}

size_t FrameRingBuffer::Capacity() const
{
    return frames.size();
}

void FrameRingBuffer::Clear()
{
    writeIndex = 0;
    count = 0;
}