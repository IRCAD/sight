/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
                                        compositor::Core::StereoModeType _stereoMode,
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
