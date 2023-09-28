/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

#include <data/TransferFunction.hpp>

#include <viz/scene3d/resource.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d
{

/**
 * @brief This binds a sight::data::TransferFunction to a shared Ogre texture. It allows to share the GPU memory from
 * multiple callers that use the same object, and only update the GPU buffer when necessary.
 */
class VIZ_SCENE3D_CLASS_API TransferFunction : public resource<data::TransferFunction,
                                                               Ogre::Texture,
                                                               TransferFunction>
{
public:

    /// Instantiates a transfer function. If a texture matching the transfer function name does not exist, it creates a
    /// new one, otherwise it wraps the existing one.
    /// @param object input transfer function
    /// @param suffixId optional suffix to the object identifier, this allows to create different copies of the object
    VIZ_SCENE3D_API TransferFunction(const data::TransferFunction::csptr& _tf, const std::string& suffixId = "");
    VIZ_SCENE3D_API ~TransferFunction();

    /// Update the TF texture according to the transfer function data.
    VIZ_SCENE3D_API void update() override;

    /// Bind the texture and the uniforms in a given pass
    template<class GPU_PARAMETERS>
    void bind(
        const Ogre::Pass* _pass,
        const std::string& _texUnitName,
        Ogre::SharedPtr<GPU_PARAMETERS> _params,
        const std::string& _uniform = "u_f3TFWindow"
    ) const;

private:

    /// Stores the tf window to upload it when necessary as a fragment shader uniform
    Ogre::Vector3 m_tfWindow;
};

//------------------------------------------------------------------------------

template<class GPU_PARAMETERS>
inline void TransferFunction::bind(
    const Ogre::Pass* const _pass,
    const std::string& _texUnitName,
    Ogre::SharedPtr<GPU_PARAMETERS> _params,
    const std::string& _uniform
) const
{
    SIGHT_ASSERT("Pass is null", _pass);
    SIGHT_ASSERT("Parameters pointer is null", _params);

    auto* texUnitState = _pass->getTextureUnitState(_texUnitName);
    SIGHT_ASSERT("'" + _texUnitName + "' texture unit is not found", texUnitState);

    if(m_resource && texUnitState->getTextureName() != m_resource->getName())
    {
        texUnitState->setTexture(m_resource);
    }

    texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);
    texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    _params->setNamedConstant(_uniform, m_tfWindow);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
