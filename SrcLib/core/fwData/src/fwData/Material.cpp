/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>
#include "fwData/Material.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Material,  ::fwData::Material);
namespace fwData {

Material::Material() : m_ambient() , m_diffuse()
{
}

Material::~Material()
{
}


Material::sptr Material::clone() const
{
	SLM_TRACE("Material::clone()");
	::fwData::Material::NewSptr pMaterial;

	pMaterial->m_ambient = this->m_ambient;
	pMaterial->m_diffuse = this->m_diffuse;

	return pMaterial;
}

Color &Material::ambient()
{
	return m_ambient ;
}

Color &Material::diffuse()
{
	return m_diffuse ;
}

const Color &Material::ambient() const
{
	return m_ambient ;
}

const Color &Material::diffuse() const
{
	return m_diffuse ;
}



}
