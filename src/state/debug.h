#pragma once

namespace StateImpl {
    namespace Debug {
        bool Init(int state);
        bool Shutdown(int state);
        bool EventTick();
        bool Tick();
    };
};
