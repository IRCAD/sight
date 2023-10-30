/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/layer.hpp"

#include <OgreCompositor.h>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Creates a compositor to compute volume ray entry points.
 */
class VIZ_SCENE3D_CLASS_API ray_entry_compositor
{
public:

    using uptr = std::unique_ptr<ray_entry_compositor>;

    /// Construstor. Creates the ogre compositor.
    VIZ_SCENE3D_API ray_entry_compositor(
        std::string _compositor_name,
        std::uint8_t _rq_group,
        compositor::core::stereo_mode_t _stereo_mode,
        bool _enable_mixed_rendering = false
    );

    /// Destructor. Deletes the created compositor.
    VIZ_SCENE3D_API virtual ~ray_entry_compositor();

    /// Returns the created compositor's name.
    [[nodiscard]] VIZ_SCENE3D_API const std::string& get_name() const;

private:

    /// Compositor created by this object.
    Ogre::CompositorPtr m_compositor;

    /// Created compositor's name.
    const std::string m_compositor_name;
};

} // namespace sight::viz::scene3d::vr
