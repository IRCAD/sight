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

const Object::FieldID Reconstruction::ID_IMAGE = "ID_IMAGE";
const Object::FieldID Reconstruction::ID_MESH = "ID_MESH";


//------------------------------------------------------------------------------

Reconstruction::Reconstruction() :
    m_bIsVisible(false),
    m_pMaterial( ::fwData::Factory::New< ::fwData::Material>() ),
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

    setField( Reconstruction::ID_IMAGE);
    setField( Reconstruction::ID_MESH);
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

    this->m_bIsVisible = _source->m_bIsVisible;
    this->m_pMaterial = _source->m_pMaterial;
    this->m_sReconstructionFormat = _source->m_sReconstructionFormat;
    this->m_sOrganName = _source->m_sOrganName;
    this->m_sStructureType = _source->m_sStructureType;
    this->m_bIsClosed = _source->m_bIsClosed;
    this->m_bIsAutomatic = _source->m_bIsAutomatic;
    this->m_dAvgVolume = _source->m_dAvgVolume;
    this->m_dVolStdDeviation = _source->m_dVolStdDeviation;
    this->m_dVolPctConfidence = _source->m_dVolPctConfidence;
    this->m_sReconstructionTime = _source->m_sReconstructionTime;
    this->m_bMaskGenerated = _source->m_bMaskGenerated;
    this->m_ui8Level = _source->m_ui8Level;
    this->m_ui16Label = _source->m_ui16Label;
    this->m_bGenerated3D = _source->m_bGenerated3D;
    this->m_sType3D = _source->m_sType3D;
    this->m_fsPath = _source->m_fsPath;
    this->m_i32DbID = _source->m_i32DbID;
}

//------------------------------------------------------------------------------

void Reconstruction::deepCopy( Reconstruction::csptr _source )
{
    this->fieldDeepCopy( _source );

    this->m_bIsVisible = _source->m_bIsVisible;
    this->m_pMaterial->deepCopy( Material::dynamicConstCast(_source->m_pMaterial) );
    this->m_sReconstructionFormat = _source->m_sReconstructionFormat;
    this->m_sOrganName = _source->m_sOrganName;
    this->m_sStructureType = _source->m_sStructureType;
    this->m_bIsClosed = _source->m_bIsClosed;
    this->m_bIsAutomatic = _source->m_bIsAutomatic;
    this->m_dAvgVolume = _source->m_dAvgVolume;
    this->m_dVolStdDeviation = _source->m_dVolStdDeviation;
    this->m_dVolPctConfidence = _source->m_dVolPctConfidence;
    this->m_sReconstructionTime = _source->m_sReconstructionTime;
    this->m_bMaskGenerated = _source->m_bMaskGenerated;
    this->m_ui8Level = _source->m_ui8Level;
    this->m_ui16Label = _source->m_ui16Label;
    this->m_bGenerated3D = _source->m_bGenerated3D;
    this->m_sType3D = _source->m_sType3D;
    this->m_fsPath = _source->m_fsPath;
    this->m_i32DbID = _source->m_i32DbID;
}

//------------------------------------------------------------------------------

void Reconstruction::setImage( ::fwData::Image::sptr _pImage )
{
    if ( _pImage == 0 )
    {
        SLM_WARN("Reconstruction::setImage : the image pointer is null.");
        this->setField( Reconstruction::ID_IMAGE ); // the previous image is deleted
    }
    else
    {
        this->setFieldSingleElement( Reconstruction::ID_IMAGE, _pImage );
    }
}

//------------------------------------------------------------------------------

::fwData::Image::csptr Reconstruction::getImage() const
{
    ::fwData::Image::csptr _pImage;

    const unsigned int nbChildren = this->getField( Reconstruction::ID_IMAGE )->children().size();

    assert ( nbChildren <= 1 );

    // Test if the image is allocated
    if( nbChildren == 1 )
    {
        _pImage = ::fwData::Image::dynamicCast ( this->getField( Reconstruction::ID_IMAGE )->children()[0] );
    }
    else
    {
        SLM_WARN("Reconstruction::getImage : return an image pointer is null.");
    }

    return _pImage;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr Reconstruction::getImage()
{
    ::fwData::Image::sptr _pImage;

    const unsigned int nbChildren = this->getField( Reconstruction::ID_IMAGE )->children().size();

    assert ( nbChildren <= 1 );

    // Test if the image is allocated
    if( nbChildren == 1 )
    {
        _pImage = ::fwData::Image::dynamicCast ( this->getField( Reconstruction::ID_IMAGE )->children()[0] );
    }
    else
    {
        SLM_WARN("Reconstruction::getImage : return an image pointer is null.");
    }

    return _pImage;
}

//------------------------------------------------------------------------------

void Reconstruction::setMesh( ::fwData::Mesh::sptr _pMesh )
{
    if ( _pMesh == 0 )
    {
        SLM_WARN("Reconstruction::setMesh : the mesh pointer is null.");
        this->setField( Reconstruction::ID_MESH ); // the previous image is deleted
        m_bIsClosed = ::boost::logic::indeterminate;
    }
    else
    {
        if (_pMesh != this->getMesh())
        {
            m_bIsClosed = ::boost::logic::indeterminate;
        }
        this->setFieldSingleElement( Reconstruction::ID_MESH, _pMesh );
    }


}

//------------------------------------------------------------------------------

::fwData::Mesh::csptr Reconstruction::getMesh() const
{
    ::fwData::Mesh::csptr _pMesh;

    const unsigned int nbChildren = this->getField( Reconstruction::ID_MESH )->children().size();

    assert ( nbChildren <= 1 );

    // Test if the image is allocated
    if( nbChildren == 1 )
    {
        _pMesh = boost::dynamic_pointer_cast< ::fwData::Mesh > ( this->getField( Reconstruction::ID_MESH )->children()[0] );
    }
    SLM_WARN_IF("Reconstruction::getMesh : return an image pointer is null.", nbChildren == 0);

    return _pMesh;
}

//------------------------------------------------------------------------------

::fwData::Mesh::sptr Reconstruction::getMesh()
{
    ::fwData::Mesh::sptr _pMesh;

    const unsigned int nbChildren = this->getField( Reconstruction::ID_MESH )->children().size();

    assert ( nbChildren <= 1 );

    // Test if the image is allocated
    if( nbChildren == 1 )
    {
        _pMesh = boost::dynamic_pointer_cast< ::fwData::Mesh > ( this->getField( Reconstruction::ID_MESH )->children()[0] );
    }
    SLM_WARN_IF("Reconstruction::getMesh : return an image pointer is null.", nbChildren == 0);

    return _pMesh;
}

//------------------------------------------------------------------------------

void Reconstruction::setMaterial( ::fwData::Material::sptr _pMaterial )
{
    m_pMaterial = _pMaterial;
}

//------------------------------------------------------------------------------

::fwData::Material::csptr Reconstruction::getMaterial() const
{
    return m_pMaterial;
}

//------------------------------------------------------------------------------

::fwData::Material::sptr Reconstruction::getMaterial()
{
    return m_pMaterial;
}

//------------------------------------------------------------------------------

bool Reconstruction::getIsClosed()
{
    bool isClosed = false;
    if (::boost::logic::indeterminate(m_bIsClosed))
    {
        m_bIsClosed = Reconstruction::isClosed(this->getSptr());
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
