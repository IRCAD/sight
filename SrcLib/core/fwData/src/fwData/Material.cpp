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
	m_ambient() ,
	m_diffuse(),
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

Material::sptr Material::clone() const
{
	SLM_TRACE("Material::clone()");
	::fwData::Material::NewSptr pMaterial;

	pMaterial->m_ambient = this->m_ambient;
	pMaterial->m_diffuse = this->m_diffuse;
	pMaterial->m_shadingMode = this->m_shadingMode;
	pMaterial->m_representationMode = this->m_representationMode;

	return pMaterial;
}

//------------------------------------------------------------------------------

Color &Material::ambient()
{
	return m_ambient ;
}

//------------------------------------------------------------------------------

Color &Material::diffuse()
{
	return m_diffuse ;
}

//------------------------------------------------------------------------------

const Color &Material::ambient() const
{
	return m_ambient ;
}

//------------------------------------------------------------------------------

const Color &Material::diffuse() const
{
	return m_diffuse ;
}



}
