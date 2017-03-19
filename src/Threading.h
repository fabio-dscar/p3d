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

        std::unique_ptr<WorkerPool> Workers = nullptr;

        static const uint32 DEFAULT_NUM_THREADS = 4;

        uint32 getNumberOfProcessors() {

            // Try querying the native APIs first
        #if _WIN32
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            if (sysInfo.dwNumberOfProcessors > 0)
                return sysInfo.dwNumberOfProcessors;
        #else
            int32 numProcs = sysconf(_SC_NPROCESSORS_ONLN);
            if (numProcs > 0)
                return numProcs;
        #endif

            // Query the standard library
            uint32 num = std::thread::hardware_concurrency();
            if (num > 0)
                return num;

            // Use default value if others fail
            return DEFAULT_NUM_THREADS;
        }

        void initThreads(int numThreads) {
            Workers = std::make_unique<WorkerPool>(numThreads);
        }


    }

}