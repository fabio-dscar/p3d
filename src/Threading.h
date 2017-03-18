#pragma once

#include <MathDefs.h>

#include <thread>
#if _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Photon {

    namespace Threading {

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
            uint32 n = std::thread::hardware_concurrency();
            if (n > 0)
                return n;

            // Use default value if others fail
            return DEFAULT_NUM_THREADS;
        }

        void initThreads(int numThreads) {
            
        }


    }

}