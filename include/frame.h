#ifndef FRAME_H
#define FRAME_H

#include <memory>
extern "C" {
#include "libavutil/frame.h"
}

using Frame = std::shared_ptr<AVFrame>;

inline Frame make_shared_frame() {
    return Frame(av_frame_alloc(), [](AVFrame* f){ av_frame_free(&f); });
}

#endif // FRAME_H
