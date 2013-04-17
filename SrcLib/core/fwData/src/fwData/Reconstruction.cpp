/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Image.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Material.hpp"
#include "fwData/Reconstruction.hpp"


fwDataRegisterMacro( ::fwData::Reconstruction );
namespace fwData
{

//------------------------------------------------------------------------------

Reconstruction::Reconstruction(::fwData::Object::Key key) :
    m_bIsVisible(false),
    m_sReconstructionFormat(""),
    m_sOrganName(""),
    m_sStructureType(""),
    m_bIsClosed( ::boost::logic::indeterminate ),
    m_bIsAutomatic(false),
    m_dAvgVolume(0),
    m_dVolStdDeviation(0),
    m_dVolPctConfidence(0),
    m_sReconstructionTime(""),
    m_bMaskGenerated(false),
    m_ui8Level(0),
    m_ui16Label(0),
    m_bGenerated3D(false),
    m_sType3D(""),
    m_fsPath(""),
    m_i32DbID(0),
    m_attrMaterial( ::fwData::factory::New< ::fwData::Material>() )
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
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_bIsVisible            = other->m_bIsVisible;
    m_sReconstructionFormat = other->m_sReconstructionFormat;
    m_sOrganName            = other->m_sOrganName;
    m_sStructureType        = other->m_sStructureType;
    m_bIsClosed             = other->m_bIsClosed;
    m_bIsAutomatic          = other->m_bIsAutomatic;
    m_dAvgVolume            = other->m_dAvgVolume;
    m_dVolStdDeviation      = other->m_dVolStdDeviation;
    m_dVolPctConfidence     = other->m_dVolPctConfidence;
    m_sReconstructionTime   = other->m_sReconstructionTime;
    m_bMaskGenerated        = other->m_bMaskGenerated;
    m_ui8Level              = other->m_ui8Level;
    m_ui16Label             = other->m_ui16Label;
    m_bGenerated3D          = other->m_bGenerated3D;
    m_sType3D               = other->m_sType3D;
    m_fsPath                = other->m_fsPath;
    m_i32DbID               = other->m_i32DbID;

    m_attrMaterial = other->m_attrMaterial;
    m_attrImage    = other->m_attrImage;
    m_attrMesh     = other->m_attrMesh;
}

//------------------------------------------------------------------------------

void Reconstruction::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Reconstruction::csptr other = Reconstruction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_bIsVisible            = other->m_bIsVisible;
    m_sReconstructionFormat = other->m_sReconstructionFormat;
    m_sOrganName            = other->m_sOrganName;
    m_sStructureType        = other->m_sStructureType;
    m_bIsClosed             = other->m_bIsClosed;
    m_bIsAutomatic          = other->m_bIsAutomatic;
    m_dAvgVolume            = other->m_dAvgVolume;
    m_dVolStdDeviation      = other->m_dVolStdDeviation;
    m_dVolPctConfidence     = other->m_dVolPctConfidence;
    m_sReconstructionTime   = other->m_sReconstructionTime;
    m_bMaskGenerated        = other->m_bMaskGenerated;
    m_ui8Level              = other->m_ui8Level;
    m_ui16Label             = other->m_ui16Label;
    m_bGenerated3D          = other->m_bGenerated3D;
    m_sType3D               = other->m_sType3D;
    m_fsPath                = other->m_fsPath;
    m_i32DbID               = other->m_i32DbID;

    m_attrMaterial = ::fwData::Object::copy(other->m_attrMaterial, cache);
    m_attrImage    = ::fwData::Object::copy(other->m_attrImage, cache);
    m_attrMesh     = ::fwData::Object::copy(other->m_attrMesh, cache);
}

//------------------------------------------------------------------------------

::boost::logic::tribool &Reconstruction::getRefIsClosed()
{
    return m_bIsClosed;
}

//------------------------------------------------------------------------------

const ::boost::logic::tribool& Reconstruction::getCRefIsClosed() const
{
    return m_bIsClosed;
}

//------------------------------------------------------------------------------

void Reconstruction::setIsClosed(::boost::logic::tribool isClosed)
{
    m_bIsClosed = isClosed;
}

//------------------------------------------------------------------------------

} // end namespace fwData
