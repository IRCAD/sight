/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/WindowManager.hpp"

#include "viz/scene3d/Utils.hpp"

#include <OgreResourceGroupManager.h>

namespace sight::viz::scene3d
{

// ----------------------------------------------------------------------------

void WindowManager::add(Ogre::RenderWindow* _window)
{
    m_windows.insert(_window);
}

// ----------------------------------------------------------------------------

void WindowManager::remove(Ogre::RenderWindow* _window)
{
    m_windows.erase(_window);
}

// ----------------------------------------------------------------------------

Ogre::RenderWindow* WindowManager::get(const std::string& _name) const
{
    auto it = std::find_if(m_windows.begin(), m_windows.end(), [&](auto _window){return _window->getName() == _name;});
    return it != m_windows.end() ? *it : nullptr;
}

// ----------------------------------------------------------------------------

viz::scene3d::WindowManager::sptr WindowManager::get()
{
    return core::LazyInstantiator<viz::scene3d::WindowManager>::getInstance();
}

// ----------------------------------------------------------------------------

bool WindowManager::hasWindow()
{
    return !m_windows.empty();
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
