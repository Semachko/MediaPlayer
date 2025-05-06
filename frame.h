#ifndef FRAME_H
#define FRAME_H

extern "C" {
    #include "libavcodec/avcodec.h"
}

class Frame
{
public:
    AVFrame* frame;

public:
    Frame() {
        frame = av_frame_alloc();
    }
    ~Frame() {
        av_frame_free(&frame);
    }

    Frame(const Frame&) = delete;
    Frame& operator=(const Frame&) = delete;

    Frame(Frame&& other) noexcept {
        frame = other.frame;
        other.frame = nullptr;
    }
    Frame& operator=(Frame&& other) noexcept {
        if (this != &other) {
            av_frame_free(&frame);
            frame = other.frame;
            other.frame = nullptr;
        }
        return *this;
    }

    AVFrame* operator->() { return frame; }
    AVFrame* get() const { return frame; }
};

#endif // FRAME_H
