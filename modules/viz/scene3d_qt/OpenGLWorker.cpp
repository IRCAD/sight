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

#include "modules/viz/scene3d_qt/OpenGLWorker.hpp"

#include "modules/viz/scene3d_qt/OpenGLContext.hpp"

#include <core/profiling.hpp>

#include <QApplication>
#include <QRunnable>
#include <QThreadPool>

#include <map>
#include <memory>
#include <mutex>

namespace sight::module::viz::scene3d_qt
{

// Wraps around a function to set the OpenGL context before calling it.
// Can be scheduled to be executed by a thread pool.
struct OpenGLRunner final : public QRunnable
{
    // Constructor.
    OpenGLRunner(open_gl_worker* _worker, sight::viz::scene3d::graphics_worker::task_t _task) :
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
        QOpenGLContext* worker_context = m_worker->m_gl_widget->context();
        if(worker_context->thread() == nullptr)
        {
            QThread* current_thread = QThread::currentThread();
            worker_context->moveToThread(current_thread);
            m_worker->m_gl_widget->makeCurrent();
        }

        m_task();
        worker_context->moveToThread(qApp->thread());
        m_worker->m_gl_widget->update();
    }

    private:

        // Pointer to the worker running the task.
        open_gl_worker* m_worker;

        // Task to run.
        sight::viz::scene3d::graphics_worker::task_t m_task;
};

//------------------------------------------------------------------------------

open_gl_worker::open_gl_worker(QOpenGLWidget* _gl_widget) :
    m_gl_widget(_gl_widget)
{
    m_thread_pool = std::make_unique<QThreadPool>();

    // Only use a single thread. We want tasks to be executed in a FIFO order.
    // Also avoids the cost of having to create an OpenGL context for every thread.
    m_thread_pool->setMaxThreadCount(1);

    auto global_gl_context = open_gl_context::get_global_ogre_open_gl_context();
    m_gl_context = std::unique_ptr<QOpenGLContext>(open_gl_context::create_ogre_gl_context(global_gl_context.get()));

    // Remove thread affinity so it can be pulled by the worker.
    m_gl_widget->context()->doneCurrent();
    m_gl_widget->context()->moveToThread(nullptr);
}

//------------------------------------------------------------------------------

open_gl_worker::~open_gl_worker()
{
    m_thread_pool->clear();       // Clear all pending tasks.
    m_thread_pool->waitForDone(); // Wait on the running task.
}

//------------------------------------------------------------------------------

void open_gl_worker::push_task(sight::viz::scene3d::graphics_worker::task_t _task)
{
    auto* runner = new OpenGLRunner(this, _task);
    runner->setAutoDelete(true); // Let the thread pool delete the runner once it has finished running.

    m_thread_pool->start(runner, QThread::Priority::HighestPriority);
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d_qt
