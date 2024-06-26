/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include <core/base.hpp>
#include <core/lazy_instantiator.hpp>

#include <OGRE/OgreRenderWindow.h>

namespace sight::viz::scene3d
{

/**
 * @brief This singleton stores all Ogre render windows and manage their deletion.
 *
 * The problem is that the OpenGLRenderSystem we use, somehow, needs the first window created to be deleted last.
 * This class allows to differ the deletion of the first created window.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API window_manager : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(window_manager, viz::scene3d::window_manager);

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Registers an Ogre window.
    SIGHT_VIZ_SCENE3D_API void add(Ogre::RenderWindow* _window);

    /// Unregisters an Ogre window.
    SIGHT_VIZ_SCENE3D_API void remove(Ogre::RenderWindow* _window);

    /// Gets an Ogre window.
    SIGHT_VIZ_SCENE3D_API Ogre::RenderWindow* get(const std::string& _name) const;

    /**
     * @brief Gets the unique instance of this class.
     * @return the singleton instance.
     */
    SIGHT_VIZ_SCENE3D_API static viz::scene3d::window_manager::sptr get();

    /**
     * @brief Returns true if there is at least one window.
     * @@return true if at least one window is registered.
     */
    SIGHT_VIZ_SCENE3D_API bool has_window();

private:

    /// Stores all Ogre windows.
    std::set<Ogre::RenderWindow*> m_windows;
};

} // namespace sight::viz::scene3d.
