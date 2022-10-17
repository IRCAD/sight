/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include <OgreMaterialManager.h>

namespace sight::viz::scene3d::compositor::listener
{

//-----------------------------------------------------------------------------

class AutoStereoCompositorListener : public Ogre::MaterialManager::Listener
{
public:

    /// Constructor, needs to know how many views are needed.
    AutoStereoCompositorListener(std::uint8_t _viewpointNumber);

    /// Destructor, destroys created techniques.
    ~AutoStereoCompositorListener() override;

    /// Callback called each time a scheme is not found
    Ogre::Technique* handleSchemeNotFound(
        std::uint16_t _schemeIndex,
        const Ogre::String& _schemeName,
        Ogre::Material* _originalMaterial,
        std::uint16_t _lodIndex,
        const Ogre::Renderable* _renderable
    ) override;

private:

    /// Pair storing techniques together with their material's name.
    using TechniqueMaterialPair = std::pair<Ogre::Technique*, std::string>;

    /// list of techniques to be removed from the material when destroying the listener
    std::vector<TechniqueMaterialPair> m_createdTechniques;

    /// Number of viewpoints used for stereo rendering.
    const std::uint8_t m_viewpointNumber;
};

} // namespace sight::viz::scene3d::compositor::listener
