#pragma once

namespace StateImpl {
    namespace None {
        bool Init(int state);
        bool Shutdown(int state);
        bool Unk(int state);
        bool Tick(int state);
    };
};
