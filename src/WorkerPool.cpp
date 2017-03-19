#include <WorkerPool.h>

using namespace Photon::Threading;

WorkerPool::WorkerPool(uint32 threadCount)
    : _numThreads(threadCount),
    _terminateFlag(false) {
    startThreads();
}

WorkerPool::~WorkerPool() {
    stop();
}

std::shared_ptr<Task> WorkerPool::getTask(uint32 &subTaskId) {
    if (_terminateFlag)
        return nullptr;

    std::shared_ptr<Task> task = _tasks.front();
    if (task->isAborting()) {
        _tasks.pop_front();
        return nullptr;
    }

    // Get next subtask id
    subTaskId = task->startSubTask();

    // If it is the last subtask, remove the task
    if (subTaskId == task->numSubTasks() - 1)
        _tasks.pop_front();

    return std::move(task);
}

void WorkerPool::runWorker(uint32 threadId) {
    while (!_terminateFlag) {
        uint32 subTaskId;
        std::shared_ptr<Task> task;

        {   // Start mutex
            std::unique_lock<std::mutex> lock(_taskMutex);
            _taskCond.wait(lock, [this](){ return _terminateFlag || !_tasks.empty(); });
            task = getTask(subTaskId);
        }   // End mutex

        if (task)
            task->run(threadId, subTaskId);
    }
}

void WorkerPool::startThreads() {
    _terminateFlag = false;

    for (uint32 i = 0; i < _numThreads; ++i) {
        _workers.emplace_back(new std::thread(&WorkerPool::runWorker, this, i));
        _idToNumericId.insert(std::make_pair(_workers.back()->get_id(), i));
    }
}

void WorkerPool::yield(Task &wait) {
    std::chrono::milliseconds waitSpan(10);
    uint32 id = _numThreads; // Threads not in the pool get a previously unassigned id

    auto iter = _idToNumericId.find(std::this_thread::get_id());
    if (iter != _idToNumericId.end())
        id = iter->second;

    while (!wait.isDone() && !_terminateFlag) {
        uint32 subTaskId;
        std::shared_ptr<Task> task;

        {
            std::unique_lock<std::mutex> lock(_taskMutex);
            if (!_taskCond.wait_for(lock, waitSpan, [this](){return _terminateFlag || !_tasks.empty(); }))
                continue;

            task = getTask(subTaskId);
        }

        if (task)
            task->run(id, subTaskId);
    }
}

void WorkerPool::reset() {
    stop();
    _tasks.clear();
    startThreads();
}

void WorkerPool::stop() {
    _terminateFlag = true;

    {   
        std::unique_lock<std::mutex> lock(_taskMutex);
        _taskCond.notify_all(); // Unlock workers
    }

    // Terminate all workers
    while (!_workers.empty()) {
        _workers.back()->detach();
        _workers.pop_back();
    }
}

std::shared_ptr<Task> WorkerPool::pushTask(TaskFunc func, int numSubtasks, Finisher finisher) {
    // Create new task
    std::shared_ptr<Task> task(std::make_shared<Task>(std::move(func), std::move(finisher), numSubtasks));

    { // Start task mutex
        std::unique_lock<std::mutex> lock(_taskMutex);
        _tasks.emplace_back(task);  // Push the task

        // Unlock the waiting workers
        if (numSubtasks == 1)
            _taskCond.notify_one();
        else
            _taskCond.notify_all();

    } // End task mutex

    return std::move(task);
}