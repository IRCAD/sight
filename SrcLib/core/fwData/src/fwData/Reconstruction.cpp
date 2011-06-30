/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include "fwData/Image.hpp"
#include "fwData/TriangularMesh.hpp"
#include "fwData/Material.hpp"
#include "fwData/Reconstruction.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Reconstruction, ::fwData::Reconstruction );
namespace fwData
{

const Object::FieldID Reconstruction::ID_IMAGE = "ID_IMAGE";
const Object::FieldID Reconstruction::ID_MESH = "ID_MESH";


//------------------------------------------------------------------------------

Reconstruction::Reconstruction() :
    m_bIsVisible(false),
    m_pMaterial( ::fwTools::Factory::New< ::fwData::Material>() ),
    m_sReconstructionFormat(""),
    m_sOrganName(""),
    m_sStructureType(""),
    m_bIsClosed(false),
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
    ::fwTools::Object::shallowCopyOfChildren( _source );

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
    ::fwTools::Object::deepCopyOfChildren( _source );

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

    const unsigned int NbChildren = this->getField( Reconstruction::ID_IMAGE )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
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

    const unsigned int NbChildren = this->getField( Reconstruction::ID_IMAGE )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
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

void Reconstruction::setTriangularMesh( ::fwData::TriangularMesh::sptr _pTriangularMesh )
{
    if ( _pTriangularMesh == 0 )
    {
        SLM_WARN("Reconstruction::setImage : the image pointer is null.");
        this->setField( Reconstruction::ID_MESH ); // the previous image is deleted
    }
    else
    {
        this->setFieldSingleElement( Reconstruction::ID_MESH, _pTriangularMesh );
    }
}

//------------------------------------------------------------------------------

::fwData::TriangularMesh::csptr Reconstruction::getTriangularMesh() const
{
    return const_cast< Reconstruction *  >(this)->getTriangularMesh();
}

//------------------------------------------------------------------------------

::fwData::TriangularMesh::sptr Reconstruction::getTriangularMesh()
{
    ::fwData::TriangularMesh::sptr _pTriangularMesh;

    const unsigned int NbChildren = this->getField( Reconstruction::ID_MESH )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
    {
        _pTriangularMesh = boost::dynamic_pointer_cast< ::fwData::TriangularMesh > ( this->getField( Reconstruction::ID_MESH )->children()[0] );
    }
    else
    {
        SLM_WARN("Reconstruction::getTriangularMesh : return an image pointer is null.");
    }
    return _pTriangularMesh;
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

} // end namespace fwData
