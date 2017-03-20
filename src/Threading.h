#pragma once

#include <thread>
#if _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <MathDefs.h>
#include <WorkerPool.h>

namespace Photon {

    namespace Threading {

        extern std::unique_ptr<WorkerPool> Workers;

        static const uint32 DEFAULT_NUM_THREADS = 4;

        uint32 getNumberOfProcessors();
        void initThreads(int numThreads);

    }

}