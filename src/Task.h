#pragma once

#include <condition_variable>
#include <functional>
#include <exception>
#include <stdexcept>
#include <atomic>
#include <thread>
#include <mutex>

#include <MathDefs.h>

namespace Photon {

    namespace Threading {

        class Task {

        typedef std::function<void(uint32, uint32, uint32)> TaskFunc;
        typedef std::function<void()> EndCallback;
        
        public:
            Task(TaskFunc func, EndCallback finisher, uint32 numSubTasks)
                : _taskFunc(std::move(func)),
                _endCallback(std::move(finisher)),
                _startedSubTasks(0),
                _finishedSubTasks(0),
                _numSubTasks(numSubTasks),
                _completed(false),
                _aborted(false) { }

            void run(uint32 threadId, uint32 taskId) {
                try {
                    _taskFunc(taskId, threadId, _numSubTasks);
                } catch (...) {
                    _exceptionPtr = std::current_exception();
                }

                uint32 num = ++_finishedSubTasks;
                if (num == _numSubTasks || (_aborted && num == _startedSubTasks))
                    finish();
            }

            void wait() {
                std::unique_lock<std::mutex> lock(_waitMutex);
                _waitCond.wait(lock, [this]() { return _completed == true; });

                if (_exceptionPtr)
                    std::rethrow_exception(_exceptionPtr);
            }

            void abort() {
                std::unique_lock<std::mutex> lock(_waitMutex);
                _aborted = true;
                _completed = _completed || _startedSubTasks == 0;
                _waitCond.notify_all();
            }

            bool isAborting() const {
                return _aborted;
            }

            bool isDone() const {
                return _completed;
            }

            uint32 startSubTask() {
                return _startedSubTasks++;
            }

            uint32 numSubTasks() const {
                return _numSubTasks;
            }

        private:           
            TaskFunc _taskFunc;
            EndCallback _endCallback;

            std::exception_ptr _exceptionPtr;
            std::atomic<uint32> _startedSubTasks;
            std::atomic<uint32> _finishedSubTasks;
            uint32 _numSubTasks;

            std::mutex _waitMutex;
            std::condition_variable _waitCond;
            std::atomic<bool> _completed, _aborted;

            void finish() {
                if (_endCallback && !_aborted)
                    _endCallback();

                std::unique_lock<std::mutex> lock(_waitMutex);
                _completed = true;
                _waitCond.notify_all();
            }
        };
    }
}