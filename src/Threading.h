#pragma once

#include <thread>
#if _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <PhotonMath.h>
#include <WorkerPool.h>

namespace Photon {

    namespace Threading {

        extern std::unique_ptr<WorkerPool> Workers;

        static const uint32 DEFAULT_NUM_THREADS = 4;

        uint32 getNumberOfProcessors();
        void initThreads(int numThreads);
        void parallelFor(uint32 start, uint32 end, uint32 partitions, std::function<void(uint32)> func);
    }

}