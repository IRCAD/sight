/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>
#include "fwData/Material.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Material,  ::fwData::Material);

namespace fwData
{

Material::Material() :
    m_ambient( Color::New() ) ,
    m_diffuse( Color::New() ),
    m_shadingMode(MODE_PHONG),
    m_representationMode(MODE_SURFACE),
    m_optionsMode(MODE_STANDARD)
{
}

//------------------------------------------------------------------------------

Material::~Material()
{
}

//------------------------------------------------------------------------------

void Material::shallowCopy( Material::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_ambient = _source->m_ambient;
    this->m_diffuse = _source->m_diffuse;

    this->m_shadingMode = _source->m_shadingMode;
    this->m_representationMode = _source->m_representationMode;
    this->m_optionsMode = _source->m_optionsMode;
}

//------------------------------------------------------------------------------

void Material::deepCopy( Material::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_ambient->deepCopy( _source->m_ambient );
    this->m_diffuse->deepCopy( _source->m_diffuse );

    this->m_shadingMode = _source->m_shadingMode;
    this->m_representationMode = _source->m_representationMode;
    this->m_optionsMode = _source->m_optionsMode;
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



}
