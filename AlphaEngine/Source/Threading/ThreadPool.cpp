/**
Copyright 2014-2015 Jason R. Wendlandt

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <thread>
#include <vector>

#include "Threading/ThreadPool.h"
#include "Threading/TaskRunner.h"
#include "Threading/ATask.h"
#include "Toolbox/ConcurrentQueue.h"
#include "Toolbox/Logger.h"

namespace alpha
{
    ThreadPool::ThreadPool()
        : m_running(true)
    { }
    ThreadPool::~ThreadPool() { }

    bool ThreadPool::Initialize()
    {
        // set the max number of hardware threads for the current machine
        // if zero/not computable, make a minimum of 1 thread.
        m_maxThreads = std::thread::hardware_concurrency();
        if (m_maxThreads <= 0)
        {
            m_maxThreads = 1;
        }
        LOG("  ThreadPool > Detected ", m_maxThreads, " max possible hardware threads.");

        // create the task queues, and return queue
        m_currentQueue = 0;
        m_pReturnQueue = std::make_shared<ConcurrentQueue<ATask *> >();

        // Create a TaskRunner thread for each hardware thread available.
        for (unsigned i = 0; i < m_maxThreads; ++i)
        {
            auto queue = std::make_shared<ConcurrentQueue<ATask *>>();
            m_vTaskRunnerQueues.push_back(queue);
            m_threads.push_back(std::thread(TaskRunner(&m_running, queue, m_pReturnQueue)));
        }

        return true;
    }

    bool ThreadPool::Shutdown()
    {
        // simply set the shared runnning boolean to false
        // and join on all threads.  Since all TaskRunner's
        // have a reference to this boolean, setting this to
        // false amounts to telling all threads to stop execution.
        m_running = false;

        // join on each thread
        while (m_threads.size() > 0)
        {
            std::thread &t = m_threads.back();
            
            if (t.joinable())
            {
                t.join();
            }
            else
            {
                LOG_ERR("Thread not joinable!");
            }

            // pop last, so we don't destruct the thread before joining
            m_threads.pop_back();
        }

        // Empty the task queue list, so that the queues can be properly
        // destructed.
        m_vTaskRunnerQueues.clear();

        return true;
    }

    bool ThreadPool::IsCurrentQueueEmpty()
    {
        bool empty = false;
        for (unsigned i = 0; i < m_maxThreads; ++i)
        {
            empty = m_vTaskRunnerQueues[i]->Empty() || empty;
        }
        return empty;
    }

    void ThreadPool::ProcessReturns()
    {
        ATask * pTask = nullptr;
        while (m_pReturnQueue->TryPop(pTask))
        {
            this->QueueTask(pTask);
        }
    }

    void ThreadPool::QueueTask(ATask * pTask)
    {
        // always add tasks to the next task queue with a round robin approach.
        m_currentQueue = (m_currentQueue + 1) % m_maxThreads;
        m_vTaskRunnerQueues[m_currentQueue]->Push(pTask);
    }
}
