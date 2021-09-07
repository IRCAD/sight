/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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
#include "viz/scene3d/Layer.hpp"

#include <OgreCompositor.h>

namespace sight::viz::scene3d
{

namespace vr
{

/**
 * @brief Creates a compositor to compute volume ray entry points.
 */
class VIZ_SCENE3D_CLASS_API RayEntryCompositor
{
public:

    using uptr = std::unique_ptr<RayEntryCompositor>;

    /// Construstor. Creates the ogre compositor.
    VIZ_SCENE3D_API RayEntryCompositor(
        const std::string& _compositorName,
        std::uint8_t _rqGroup,
        compositor::Core::StereoModeType _stereoMode,
        bool _enableMixedRendering = false
    );

    /// Destructor. Deletes the created compositor.
    VIZ_SCENE3D_API virtual ~RayEntryCompositor();

    /// Returns the created compositor's name.
    VIZ_SCENE3D_API const std::string& getName() const;

private:

    /// Compositor created by this object.
    ::Ogre::CompositorPtr m_compositor;

    /// Created compositor's name.
    const std::string m_compositorName;
};

} // namespace vr

} // namespace sight::viz::scene3d
