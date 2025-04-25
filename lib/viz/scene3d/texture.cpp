/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "viz/scene3d/texture.hpp"

#include <viz/scene3d/detail/texture_manager.hpp>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

texture::texture(const data::image::csptr& _image, const std::string& _suffix_id)
{
    m_resource = detail::texture_manager::get()->instantiate(_image, _suffix_id);
}

//-----------------------------------------------------------------------------

texture::~texture()
{
    if(m_resource)
    {
        detail::texture_manager::get()->release(m_resource);
    }
}

//-----------------------------------------------------------------------------

void texture::update()
{
    m_window = viz::scene3d::detail::texture_manager::get()->load(m_resource).second;
}

//-----------------------------------------------------------------------------

void texture::set_dirty()
{
    if(m_resource)
    {
        m_resource->touch();
    }
}

//------------------------------------------------------------------------------

void texture::bind(
    Ogre::TextureUnitState* _tex_unit,
    Ogre::TextureType _type,
    Ogre::TextureFilterOptions _filter_type,
    Ogre::TextureAddressingMode _address_mode
) const
{
    SIGHT_ASSERT("The texture unit is null.", _tex_unit);

    _tex_unit->setTextureName(m_resource->getName(), _type);
    _tex_unit->setTextureFiltering(_filter_type);
    _tex_unit->setTextureAddressingMode(_address_mode);
}

//------------------------------------------------------------------------------

void texture::bind(
    Ogre::Pass* _pass,
    const std::string& _sampler_name,
    std::optional<Ogre::TextureFilterOptions> _filter_type,
    std::optional<Ogre::TextureAddressingMode> _address_mode,
    bool _upload_window_uniform
) const
{
    SIGHT_ASSERT("The pass is null.", _pass);

    Ogre::TextureUnitState* tex_unit = _pass->getTextureUnitState(_sampler_name);
    SIGHT_ASSERT("The sampler '" + _sampler_name + "' cannot be retrieved.", tex_unit);

    if(_filter_type != std::nullopt)
    {
        tex_unit->setTextureFiltering(_filter_type.value());
    }

    if(_address_mode != std::nullopt)
    {
        tex_unit->setTextureAddressingMode(_address_mode.value());
    }

    if(m_resource)
    {
        tex_unit->setTexture(m_resource);
        tex_unit->setTextureName(m_resource->getName(), m_resource->getTextureType());
    }

    auto fp_params = _pass->getFragmentProgramParameters();
    if(_upload_window_uniform && fp_params && (fp_params->_findNamedConstantDefinition("u_window") != nullptr))
    {
        fp_params->setNamedConstant("u_window", this->window());
    }
}

//------------------------------------------------------------------------------

void texture::create_binding(
    Ogre::Pass* _pass,
    const std::string& _sampler_name,
    const std::string& _uniform_name,
    std::optional<Ogre::TextureFilterOptions> _filter_type,
    std::optional<Ogre::TextureAddressingMode> _address_mode
) const
{
    SIGHT_ASSERT("The pass is null.", _pass);

    Ogre::TextureUnitState* const tex_unit = _pass->createTextureUnitState(_sampler_name);
    SIGHT_ASSERT("The sampler '" + _sampler_name + "' cannot be retrieved.", tex_unit);

    if(_filter_type != std::nullopt)
    {
        tex_unit->setTextureFiltering(_filter_type.value());
    }

    if(_address_mode != std::nullopt)
    {
        tex_unit->setTextureAddressingMode(_address_mode.value());
    }

    if(m_resource)
    {
        tex_unit->setTexture(m_resource);
        tex_unit->setTextureName(m_resource->getName(), m_resource->getTextureType());
    }

    auto fp_params      = _pass->getFragmentProgramParameters();
    auto tex_unit_index = _pass->getTextureUnitStateIndex(tex_unit);
    fp_params->setNamedConstant(_uniform_name, tex_unit_index);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
