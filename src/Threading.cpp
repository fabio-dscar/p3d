#include <Threading.h>

using namespace Photon;
using namespace Photon::Threading;

std::unique_ptr<WorkerPool> Photon::Threading::Workers = nullptr;

uint32 Photon::Threading::getNumberOfProcessors() {

#ifdef PHOTON_DEBUG
    return 1;
#endif

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

void Photon::Threading::initThreads(int numThreads) {
    Workers = std::make_unique<WorkerPool>(numThreads);
}

void Photon::Threading::parallelFor(uint32 start, uint32 end, uint32 partitions, std::function<void(uint32)> func) {
    auto taskRun = [&func, start, end](uint32 idx, uint32 /*threadId*/, uint32 num) {
        uint32 span = (end - start + num - 1) / num;
        uint32 iStart = start + span*idx;
        uint32 iEnd = std::min(iStart + span, end);
        for (uint32 i = iStart; i < iEnd; ++i)
            func(i);
    };

    if (partitions == 1)
        taskRun(0, 1, 0);
    else
        Workers->yield(*Workers->pushTask(taskRun, partitions));
}
