/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#pragma once

#include "modules/viz/scene3d_qt/config.hpp"

#include <viz/scene3d/graphics_worker.hpp>

#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QPointer>
#include <QSurface>
#include <QThreadPool>

namespace sight::module::viz::scene3d_qt
{

struct OpenGLRunner;

/**
 * @brief Runs OpenGL resource handling tasks in a separate thread.
 *
 * @warning can not be used for parallel rendering as it is not supported by OGRE.
 * OpenGL workers should mainly be used to fill large gpu buffers in the background.
 */
class open_gl_worker final : public sight::viz::scene3d::graphics_worker
{
public:

    /// Builds a graphic worker able to handle resources for the given surface.
    open_gl_worker(QOpenGLWidget* _gl_widget);

    /// Clears all waiting tasks and waits for the one being executed.
    ~open_gl_worker() final;

    /// Schedules a new task for the thread pool.
    void push_task(sight::viz::scene3d::graphics_worker::task_t _task) override;

private:

    friend struct OpenGLRunner;

    /// Contains an OpenGL context used by the graphics worker.
    std::unique_ptr<QOpenGLContext> m_gl_context;

    /// Contains a thread pool launching workers, this spares the expense of re-instancing a QThread with an
    /// OpenGL context for each new task.
    std::unique_ptr<QThreadPool> m_thread_pool;

    /// Contains the surface on which the thread's context is enabled.
//     QSurface* m_surface;
    QOpenGLWidget* m_gl_widget;
};

} // namespace sight::module::viz::scene3d_qt.
