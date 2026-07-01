
#pragma once

namespace StateImpl {
    namespace Intro {
        bool Init(int state);
        bool Shutdown(int state);
        bool EventTick();
        bool Tick();
    };
};
