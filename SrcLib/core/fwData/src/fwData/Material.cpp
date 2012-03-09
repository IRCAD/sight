/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Material.hpp"

fwDataRegisterMacro( ::fwData::Material );

namespace fwData
{

Material::Material() :
    m_shadingMode(MODE_PHONG),
    m_representationMode(MODE_SURFACE),
    m_optionsMode(MODE_STANDARD),
    m_ambient( Color::New() ),
    m_diffuse( Color::New() )
{
}

//------------------------------------------------------------------------------

Material::~Material()
{
}

//------------------------------------------------------------------------------

void Material::shallowCopy( Material::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_ambient = _source->m_ambient;
    m_diffuse = _source->m_diffuse;

    m_shadingMode = _source->m_shadingMode;
    m_representationMode = _source->m_representationMode;
    m_optionsMode = _source->m_optionsMode;
}

//------------------------------------------------------------------------------

void Material::deepCopy( Material::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_ambient->deepCopy( _source->m_ambient );
    m_diffuse->deepCopy( _source->m_diffuse );

    m_shadingMode = _source->m_shadingMode;
    m_representationMode = _source->m_representationMode;
    m_optionsMode = _source->m_optionsMode;
}

//------------------------------------------------------------------------------

Color::sptr Material::ambient() const
{
    return m_ambient ;
}

//------------------------------------------------------------------------------

Color::sptr Material::diffuse() const
{
    return m_diffuse ;
}

//------------------------------------------------------------------------------

void Material::setAmbient(Color::sptr ambient)
{
    m_ambient = ambient;
}

//------------------------------------------------------------------------------

void Material::setDiffuse(Color::sptr diffuse)
{
    m_diffuse = diffuse;
}

//------------------------------------------------------------------------------

}
