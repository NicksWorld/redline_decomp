#pragma once

namespace StateImpl {
    namespace None {
        bool Init(int state);
        bool Shutdown(int state);
        bool EventTick();
        bool Tick();
    };
};
