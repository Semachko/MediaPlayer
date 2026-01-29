#pragma once
#include "params/mediaparameters.h"

class ScopedPause {
    public:
        ScopedPause(MediaParameters* params_) :
                params(params_) {
            if (!params->isPaused)
                params->setIsPaused(true);
        }
        ~ScopedPause() {
            if (params->isPaused)
                params->setIsPaused(false);
        }

    private:
        MediaParameters* params;
};