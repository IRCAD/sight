/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuOgreQt/OpenGLWorker.hpp"

#include "visuOgreQt/OpenGLContext.hpp"

#include <fwCore/Profiling.hpp>

#include <QRunnable>
#include <QThreadPool>

#include <map>
#include <memory>
#include <mutex>

namespace visuOgreQt
{

// Wraps around a function to set the OpenGL context before calling it.
// Can be scheduled to be executed by a thread pool.
struct OpenGLRunner : public QRunnable
{
    // Constructor.
    OpenGLRunner(OpenGLWorker* _worker, ::fwRenderOgre::IGraphicsWorker::TaskType _task) :
        m_worker(_worker),
        m_task(_task)
    {
    }

//------------------------------------------------------------------------------

    // Destructor.
    virtual ~OpenGLRunner() final
    {
    }

//------------------------------------------------------------------------------

    // Pulls the GL context if it is not owned by the thread and runs the task.
    virtual void run() final
    {
        QOpenGLContext* workerContext = m_worker->m_glContext.get();
        if (workerContext->thread() == nullptr)
        {
            QThread* currentThread = QThread::currentThread();
            workerContext->moveToThread(currentThread);
            workerContext->makeCurrent(m_worker->m_surface);
        }

        m_task();
    }

    private:

        // Pointer to the worker running the task.
        OpenGLWorker* m_worker;

        // Task to run.
        ::fwRenderOgre::IGraphicsWorker::TaskType m_task;

};

//------------------------------------------------------------------------------

OpenGLWorker::OpenGLWorker(QSurface* _surface) :
    m_surface(_surface)
{
    m_threadPool = std::make_unique<QThreadPool>();

    // Only use a single thread. We want tasks to be executed in a FIFO order.
    // Also avoids the cost of having to create an OpenGL context for every thread.
    m_threadPool->setMaxThreadCount(1);

    auto globalGLContext = OpenGLContext::getGlobalOgreOpenGLContext();
    m_glContext = std::unique_ptr<QOpenGLContext>(OpenGLContext::createOgreGLContext(globalGLContext.get()));

    // Remove thread affinity so it can be pulled by the worker.
    m_glContext->doneCurrent();
    m_glContext->moveToThread(nullptr);
}

//------------------------------------------------------------------------------

OpenGLWorker::~OpenGLWorker()
{
    m_threadPool->clear(); // Clear all pending tasks.
    m_threadPool->waitForDone(); // Wait on the running task.
}

//------------------------------------------------------------------------------

void OpenGLWorker::pushTask(::fwRenderOgre::IGraphicsWorker::TaskType _task)
{
    OpenGLRunner* runner = new OpenGLRunner(this, _task);
    runner->setAutoDelete(true); // Let the thread pool delete the runner once it has finished running.

    m_threadPool->start(runner, QThread::Priority::HighestPriority);
}

//------------------------------------------------------------------------------

} // namespace visuOgreQt
