/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <QOpenGLContext>

#include <memory>

namespace sight::module::viz::scene3d_qt
{

/// Defines a static class to manage the OpenGL context shared by all render windows.
class open_gl_context final
{
public:

    /// Retrieves a shared pointer to Ogre's OpenGL context, creates it if does not exist or has expired.
    static std::shared_ptr<QOpenGLContext> get_global_ogre_open_gl_context();

    /**
     * @brief Creates an OpenGL 4.1 context.
     *
     * @param _shared_context context to share resources with or nullptr for no context sharing.
     * @pre if a _sharedContext is used then it must be on the same thread as the one calling this method.
     */
    static QOpenGLContext* create_ogre_gl_context(QOpenGLContext* _shared_context = nullptr);

private:

    /// Contains a weak reference to the OpenGL context, expires when no more windows hold the context.
    static std::weak_ptr<QOpenGLContext> s_global_ogre_open_gl_context;
};

} // namespace sight::module::viz::scene3d_qt.
