/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include "modules/viz/scene3dQt/config.hpp"

#include <viz/scene3d/IGraphicsWorker.hpp>

#include <QOpenGLContext>
#include <QPointer>
#include <QSurface>
#include <QThreadPool>

namespace sight::module::viz::scene3dQt
{

struct OpenGLRunner;

/**
 * @brief Runs OpenGL resource handling tasks in a separate thread.
 *
 * @warning can not be used for parallel rendering as it is not supported by OGRE.
 * OpenGL workers should mainly be used to fill large gpu buffers in the background.
 */
class OpenGLWorker final : public ::sight::viz::scene3d::IGraphicsWorker
{
public:

    /// Builds a graphic worker able to handle resources for the given surface.
    OpenGLWorker(QSurface* _surface);

    /// Clears all waiting tasks and waits for the one being executed.
    ~OpenGLWorker() final;

    /// Schedules a new task for the thread pool.
    virtual void pushTask(sight::viz::scene3d::IGraphicsWorker::TaskType _task);

private:

    friend struct OpenGLRunner;

    /// Contains an OpenGL context used by the graphics worker.
    std::unique_ptr<QOpenGLContext> m_glContext;

    /// Contains a thread pool launching workers, this spares the expense of re-instancing a QThread with an
    /// OpenGL context for each new task.
    std::unique_ptr<QThreadPool> m_threadPool;

    /// Contains the surface on which the thread's context is enabled.
    QSurface* m_surface;
};

} // namespace sight::module::viz::scene3dQt.
