/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Image.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Material.hpp"

#include "fwData/Reconstruction.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Reconstruction );
namespace fwData
{

const double Reconstruction::s_NO_COMPUTED_MASK_VOLUME = -1.;

//------------------------------------------------------------------------------

Reconstruction::Reconstruction(::fwData::Object::Key key) :
    m_bIsVisible(false),
    m_sOrganName(""),
    m_sStructureType(""),
    m_material( ::fwData::factory::New< ::fwData::Material>() ),
    m_computedMaskVolume(Reconstruction::s_NO_COMPUTED_MASK_VOLUME)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Reconstruction::shallowCopy(const Object::csptr &_source )
{
    Reconstruction::csptr other = Reconstruction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = other->m_material;
    m_image    = other->m_image;
    m_mesh     = other->m_mesh;

    m_computedMaskVolume = other->m_computedMaskVolume;
}

//------------------------------------------------------------------------------

void Reconstruction::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Reconstruction::csptr other = Reconstruction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = ::fwData::Object::copy(other->m_material, cache);
    m_image    = ::fwData::Object::copy(other->m_image, cache);
    m_mesh     = ::fwData::Object::copy(other->m_mesh, cache);

    m_computedMaskVolume = other->m_computedMaskVolume;
}

//------------------------------------------------------------------------------

} // end namespace fwData
