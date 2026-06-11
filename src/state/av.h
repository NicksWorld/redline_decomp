
#pragma once

namespace StateImpl {
    namespace AV {
        bool Init(int state);
        bool Shutdown(int state);
        bool EventTick();
        bool Tick();
    };
};
