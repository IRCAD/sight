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

#include <QOpenGLContext>

#include <memory>

namespace visuOgreQt
{

/**
 * @brief Static class to manage the OpenGL context shared by all render windows.
 */
class OpenGLContext
{
public:

    /// Retrieves a shared pointer to Ogre's OpenGL context, creates it if does not exist or has expired.
    static std::shared_ptr<QOpenGLContext> getGlobalOgreOpenGLContext();

    /// Creates an OpenGL 4.1 context.
    static QOpenGLContext* createOgreGLContext();

private:

    /// Weak reference to the OpenGL context, expires when no more windows hold the context.
    static std::weak_ptr<QOpenGLContext> s_globalOgreOpenGLContext;

};

} // namespace visuOgreQt
