/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "modules/viz/scene3dQt/OpenGLWorker.hpp"

#include "modules/viz/scene3dQt/OpenGLContext.hpp"

#include <core/Profiling.hpp>

#include <QApplication>
#include <QRunnable>
#include <QThreadPool>

#include <map>
#include <memory>
#include <mutex>

namespace sight::module::viz::scene3dQt
{

// Wraps around a function to set the OpenGL context before calling it.
// Can be scheduled to be executed by a thread pool.
struct OpenGLRunner final : public QRunnable
{
    // Constructor.
    OpenGLRunner(OpenGLWorker* _worker, sight::viz::scene3d::IGraphicsWorker::TaskType _task) :
        m_worker(_worker),
        m_task(std::move(_task))
    {
    }

//------------------------------------------------------------------------------

    // Destructor.
    ~OpenGLRunner() final =
        default;

//------------------------------------------------------------------------------

    // Pulls the GL context if it is not owned by the thread and runs the task.
    void run() final
    {
        QOpenGLContext* workerContext = m_worker->m_glWidget->context();
        if(workerContext->thread() == nullptr)
        {
            QThread* currentThread = QThread::currentThread();
            workerContext->moveToThread(currentThread);
            m_worker->m_glWidget->makeCurrent();
        }

        m_task();
        workerContext->moveToThread(qApp->thread());
        m_worker->m_glWidget->update();
    }

    private:

        // Pointer to the worker running the task.
        OpenGLWorker* m_worker;

        // Task to run.
        sight::viz::scene3d::IGraphicsWorker::TaskType m_task;
};

//------------------------------------------------------------------------------

OpenGLWorker::OpenGLWorker(QOpenGLWidget* _glWidget) :
    m_glWidget(_glWidget)
{
    m_threadPool = std::make_unique<QThreadPool>();

    // Only use a single thread. We want tasks to be executed in a FIFO order.
    // Also avoids the cost of having to create an OpenGL context for every thread.
    m_threadPool->setMaxThreadCount(1);

    auto globalGLContext = OpenGLContext::getGlobalOgreOpenGLContext();
    m_glContext = std::unique_ptr<QOpenGLContext>(OpenGLContext::createOgreGLContext(globalGLContext.get()));

    // Remove thread affinity so it can be pulled by the worker.
    m_glWidget->context()->doneCurrent();
    m_glWidget->context()->moveToThread(nullptr);
}

//------------------------------------------------------------------------------

OpenGLWorker::~OpenGLWorker()
{
    m_threadPool->clear();       // Clear all pending tasks.
    m_threadPool->waitForDone(); // Wait on the running task.
}

//------------------------------------------------------------------------------

void OpenGLWorker::pushTask(sight::viz::scene3d::IGraphicsWorker::TaskType _task)
{
    auto* runner = new OpenGLRunner(this, _task);
    runner->setAutoDelete(true); // Let the thread pool delete the runner once it has finished running.

    m_threadPool->start(runner, QThread::Priority::HighestPriority);
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3dQt
