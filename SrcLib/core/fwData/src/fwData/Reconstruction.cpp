/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"


#include <fwMath/MeshFunctions.hpp>

#include "fwData/Image.hpp"
#include "fwData/Mesh.hpp"
#include "fwData/Material.hpp"
#include "fwData/Reconstruction.hpp"


fwDataRegisterMacro( ::fwData::Reconstruction );
namespace fwData
{


//------------------------------------------------------------------------------

Reconstruction::Reconstruction() :
    m_bIsVisible(false),
    m_attrMaterial( ::fwData::Factory::New< ::fwData::Material>() ),
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
    m_i32DbID(0)
{
    SLM_TRACE_FUNC();

}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Reconstruction::shallowCopy( Reconstruction::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_bIsVisible            = _source->m_bIsVisible;
    m_sReconstructionFormat = _source->m_sReconstructionFormat;
    m_sOrganName            = _source->m_sOrganName;
    m_sStructureType        = _source->m_sStructureType;
    m_bIsClosed             = _source->m_bIsClosed;
    m_bIsAutomatic          = _source->m_bIsAutomatic;
    m_dAvgVolume            = _source->m_dAvgVolume;
    m_dVolStdDeviation      = _source->m_dVolStdDeviation;
    m_dVolPctConfidence     = _source->m_dVolPctConfidence;
    m_sReconstructionTime   = _source->m_sReconstructionTime;
    m_bMaskGenerated        = _source->m_bMaskGenerated;
    m_ui8Level              = _source->m_ui8Level;
    m_ui16Label             = _source->m_ui16Label;
    m_bGenerated3D          = _source->m_bGenerated3D;
    m_sType3D               = _source->m_sType3D;
    m_fsPath                = _source->m_fsPath;
    m_i32DbID               = _source->m_i32DbID;

    m_attrMaterial = _source->m_attrMaterial;
    m_attrImage    = _source->m_attrImage;
    m_attrMesh     = _source->m_attrMesh;
    }

//------------------------------------------------------------------------------

void Reconstruction::deepCopy( Reconstruction::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_bIsVisible            = _source->m_bIsVisible;
    m_sReconstructionFormat = _source->m_sReconstructionFormat;
    m_sOrganName            = _source->m_sOrganName;
    m_sStructureType        = _source->m_sStructureType;
    m_bIsClosed             = _source->m_bIsClosed;
    m_bIsAutomatic          = _source->m_bIsAutomatic;
    m_dAvgVolume            = _source->m_dAvgVolume;
    m_dVolStdDeviation      = _source->m_dVolStdDeviation;
    m_dVolPctConfidence     = _source->m_dVolPctConfidence;
    m_sReconstructionTime   = _source->m_sReconstructionTime;
    m_bMaskGenerated        = _source->m_bMaskGenerated;
    m_ui8Level              = _source->m_ui8Level;
    m_ui16Label             = _source->m_ui16Label;
    m_bGenerated3D          = _source->m_bGenerated3D;
    m_sType3D               = _source->m_sType3D;
    m_fsPath                = _source->m_fsPath;
    m_i32DbID               = _source->m_i32DbID;

    m_attrMaterial = ::fwData::Object::copy(_source->m_attrMaterial);
    m_attrImage    = ::fwData::Object::copy(_source->m_attrImage);
    m_attrMesh     = ::fwData::Object::copy(_source->m_attrMesh);
    }



//------------------------------------------------------------------------------

bool Reconstruction::getIsClosed()
{
    bool isClosed = false;
    // Hack until 'is closed' computation stays here
    ::fwData::Mesh::sptr mesh = m_meshForWhichISCompoutedIsClosed.lock();
    if ( mesh != this->getMesh() || ::boost::logic::indeterminate(m_bIsClosed))
    {
        m_bIsClosed = Reconstruction::isClosed(this->getSptr());
        m_meshForWhichISCompoutedIsClosed = this->getMesh();
    }
    isClosed = m_bIsClosed;
    return isClosed;
}

//------------------------------------------------------------------------------

bool Reconstruction::isClosed(Reconstruction::csptr reconstruction)
{

    bool isClosed = false;
    ::fwData::Mesh::csptr mesh = reconstruction->getMesh();
    ::fwData::Array::sptr cellData = mesh->getCellDataArray();
    ::fwData::Array::sptr cellDataOffsets = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellTypes = mesh->getCellTypesArray();
    ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();
    ::fwData::Mesh::Id nbOfCells = mesh->getNumberOfCells();
    ::fwData::Mesh::CellValueType* cellDataBegin = cellData->begin< ::fwData::Mesh::CellValueType >();
    ::fwData::Mesh::CellValueType* cellDataEnd = cellDataBegin + cellDataSize;
    ::fwData::Mesh::CellDataOffsetType* cellDataOffsetsBegin = cellDataOffsets->begin< ::fwData::Mesh::CellDataOffsetType >();
    ::fwData::Mesh::CellDataOffsetType* cellDataOffsetsEnd = cellDataOffsetsBegin + nbOfCells;
    ::fwData::Mesh::CellTypes* cellTypesBegin = cellTypes->begin< ::fwData::Mesh::CellTypes >();

    isClosed = ::fwMath::isBorderlessSurface(cellDataBegin, cellDataEnd, cellDataOffsetsBegin, cellDataOffsetsEnd, cellTypesBegin );
    return isClosed;
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
