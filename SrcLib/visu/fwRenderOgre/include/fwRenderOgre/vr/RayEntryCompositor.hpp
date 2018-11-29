/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/Layer.hpp"

#include <OgreCompositor.h>

namespace fwRenderOgre
{
namespace vr
{

/**
 * @brief Creates a compositor to compute volume ray entry points.
 */
class FWRENDEROGRE_CLASS_API RayEntryCompositor
{
public:

    using uptr = std::unique_ptr< RayEntryCompositor >;

    /// Construstor. Creates the ogre compositor.
    FWRENDEROGRE_API RayEntryCompositor(const std::string& _compositorName, std::uint8_t _rqGroup,
                                        Layer::StereoModeType _stereoMode,
                                        bool _enableMixedRendering = false);

    /// Destructor. Deletes the created compositor.
    FWRENDEROGRE_API virtual ~RayEntryCompositor();

    /// Returns the created compositor's name.
    FWRENDEROGRE_API const std::string& getName() const;

private:

    /// Compositor created by this object.
    ::Ogre::CompositorPtr m_compositor;

    /// Created compositor's name.
    const std::string m_compositorName;
};

} // namespace vr
} // namespace fwRenderOgre
