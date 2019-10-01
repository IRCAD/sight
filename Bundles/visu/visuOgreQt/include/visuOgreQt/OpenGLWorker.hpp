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

#pragma once

#include "visuOgreQt/config.hpp"

#include <fwRenderOgre/IGraphicsWorker.hpp>

#include <QOpenGLContext>
#include <QPointer>
#include <QSurface>
#include <QThreadPool>

namespace visuOgreQt
{

struct OpenGLRunner;

/**
 * @brief Runs OpenGL resource handling tasks in a separate thread.
 *
 * /!\ DISCLAIMER: can not be used for parallel rendering as it is not supported by OGRE.
 * OpenGL workers should mainly be used to fill large gpu buffers in the background.
 */
class OpenGLWorker : public ::fwRenderOgre::IGraphicsWorker
{
public:

    /// Constructor, builds a graphic worker able to handle resources for the given surface.
    OpenGLWorker(QSurface* _surface);

    /// Destructor, clears all waiting tasks and waits on the one being executed.
    virtual ~OpenGLWorker() final;

    /// Schedules a new task for the thread pool.
    virtual void pushTask(::fwRenderOgre::IGraphicsWorker::TaskType _task) final;

private:

    friend struct OpenGLRunner;

    /// OpenGL context used by the graphics worker.
    std::unique_ptr< QOpenGLContext > m_glContext;

    /// Thread pool launching workers, this spares the expense of re-instancing a QThread with an
    /// OpenGL context for each new task.
    std::unique_ptr< QThreadPool > m_threadPool;

    /// Surface on which the thread's context is enabled.
    QSurface* m_surface;

};

} // namespace visuOgreQt
