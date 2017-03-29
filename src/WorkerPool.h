#pragma once

#include <condition_variable>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>

#include <MathDefs.h>
#include <Task.h>

namespace Photon {

    namespace Threading {

        class WorkerPool {

        typedef std::function<void(uint32, uint32, uint32)> TaskFunc;
        typedef std::function<void()> EndCallback;

        public:
            WorkerPool(uint32 numThreads);
            ~WorkerPool();

            void yield(Task &wait);

            void reset();
            void stop();

            std::shared_ptr<Task> pushTask(TaskFunc func, uint32 numSubtasks = 1,
                                           EndCallback finisher = EndCallback());

            uint32 numThreads() const {
                return _numThreads;
            }

        private:
            uint32 _numThreads;
            std::vector<std::unique_ptr<std::thread>> _workers;
            std::atomic<bool> _shutdown;

            std::deque<std::shared_ptr<Task>> _tasks;
            std::mutex _taskMutex;
            std::condition_variable _taskCond;

            std::unordered_map<std::thread::id, uint32> _idToNumericId;

            std::shared_ptr<Task> getTask(uint32 &subTaskId);
            void runWorker(uint32 threadId);
            void startThreads();
        };

    }

}