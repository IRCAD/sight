/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/transfer_function.hpp>

#include <viz/scene3d/resource.hpp>

#include <OGRE/OgrePass.h>
#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d
{

/**
 * @brief This binds a sight::data::transfer_function to a shared Ogre texture. It allows to share the GPU memory from
 * multiple callers that use the same object, and only update the GPU buffer when necessary.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API transfer_function : public resource<data::transfer_function,
                                                                      Ogre::Texture,
                                                                      transfer_function>
{
public:

    /// Instantiates a transfer function. If a texture matching the transfer function name does not exist, it creates a
    /// new one, otherwise it wraps the existing one.
    /// @param _tf input transfer function
    /// @param _suffix_id optional suffix to the object identifier, this allows to create different copies of the object
    SIGHT_VIZ_SCENE3D_API transfer_function(
        const data::transfer_function::csptr& _tf,
        const std::string& _suffix_id = ""
    );
    SIGHT_VIZ_SCENE3D_API ~transfer_function();

    /// Update the TF texture according to the transfer function data.
    SIGHT_VIZ_SCENE3D_API void update() override;

    /// Bind the texture and the uniforms in a given pass
    template<class GPU_PARAMETERS>
    void bind(
        const Ogre::Pass* _pass,
        const std::string& _tex_unit_name,
        std::shared_ptr<GPU_PARAMETERS> _params,
        const std::string& _uniform = "u_f3TFWindow"
    ) const;

    /// Stores the tf window to upload it when necessary as a fragment shader uniform
    Ogre::Vector3 window;
};

//------------------------------------------------------------------------------

template<class GPU_PARAMETERS>
inline void transfer_function::bind(
    const Ogre::Pass* const _pass,
    const std::string& _tex_unit_name,
    std::shared_ptr<GPU_PARAMETERS> _params,
    const std::string& _uniform
) const
{
    SIGHT_ASSERT("Pass is null", _pass);
    SIGHT_ASSERT("Parameters pointer is null", _params);

    auto* tex_unit_state = _pass->getTextureUnitState(_tex_unit_name);
    SIGHT_ASSERT("'" + _tex_unit_name + "' texture unit is not found", tex_unit_state);

    if(m_resource && tex_unit_state->getTextureName() != m_resource->getName())
    {
        tex_unit_state->setTexture(m_resource);
    }

    tex_unit_state->setTextureFiltering(Ogre::TFO_BILINEAR);
    tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    _params->setNamedConstant(_uniform, window);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
