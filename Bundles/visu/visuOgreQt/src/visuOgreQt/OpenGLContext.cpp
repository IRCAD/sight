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

#include "visuOgreQt/OpenGLContext.hpp"

#include <fwCore/spyLog.hpp>

#include <QOpenGLFunctions_4_1_Core>

namespace visuOgreQt
{

std::weak_ptr<QOpenGLContext> OpenGLContext::s_globalOgreOpenGLContext;

//-----------------------------------------------------------------------------

QOpenGLContext* OpenGLContext::createOgreGLContext(QOpenGLContext* _sharedContext)
{
    auto* glContext = new QOpenGLContext();
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    // Enforce core profile (required on OSX).
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    // We limit ourselves to version 4.1 as it is the last one supported by Apple.
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    glContext->setFormat(format);

    if(_sharedContext)
    {
        // For context sharing to be enabled, both contexts need to be on the same thread when calling 'create'.
        // (At least on Windows: see https://community.khronos.org/t/wglsharelists-failing/42656).
        OSLM_FATAL_IF("Shared contexts should be created on the same thread.",
                      glContext->thread() != _sharedContext->thread());
        glContext->setShareContext(_sharedContext);
    }

    const bool success = glContext->create();
    OSLM_FATAL_IF("Did not manage to create an OpenGL 4.1 core context for ogre.",
                  !success || glContext->versionFunctions<QOpenGLFunctions_4_1_Core>() == nullptr);

    return glContext;
}

//-----------------------------------------------------------------------------

std::shared_ptr<QOpenGLContext> OpenGLContext::getGlobalOgreOpenGLContext()
{
    std::shared_ptr<QOpenGLContext> globalContext = s_globalOgreOpenGLContext.lock();

    if(globalContext == nullptr)
    {
        globalContext             = std::shared_ptr<QOpenGLContext>(createOgreGLContext());
        s_globalOgreOpenGLContext = globalContext;
    }

    return globalContext;
}

//-----------------------------------------------------------------------------

}
