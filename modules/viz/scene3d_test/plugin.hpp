/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/modules/viz/scene3d_test/config.hpp>

#include <core/runtime/plugin.hpp>

#include <OgreRoot.h>

namespace sight::module::viz::scene3d::test
{

/**
 * @brief   This class instantiates and destroy an Ogre::Root, which is useful for
 */
class plugin : public core::runtime::plugin
{
public:

    /// Destructor. Do nothing.
    ~plugin() noexcept override;

    /// Instantiates Ogre Root.
    SIGHT_MODULE_VIZ_SCENE3D_TEST_API void start() override;

    /// Overrides stop method. Do nothing
    SIGHT_MODULE_VIZ_SCENE3D_TEST_API void stop() noexcept override;

private:

    /// Ogre root to setup the scene and instantiate 'Text' objects.
    Ogre::Root* m_ogre_root {nullptr};
};

} // namespace sight::module::viz::scene3d::test
