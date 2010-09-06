/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Image.hpp"
#include "fwData/TriangularMesh.hpp"
#include <fwTools/Factory.hpp>

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
//  m_dRed(0),
//  m_dGreen(0),
//  m_dBlue(0),
//  m_dAlpha(0),
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
    SLM_WARN("::fwData::Reconstruction() : (ToDo)");

    setField( Reconstruction::ID_IMAGE);
    setField( Reconstruction::ID_MESH);
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction()
{
    SLM_WARN("::fwData::~Reconstruction() : (ToDo)");
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

//Reconstruction &Reconstruction::operator=(const Reconstruction & _reconstruction)
//{
//
//    this->m_bIsVisible = _reconstruction.m_bIsVisible;
//
//    this->setMaterial( _reconstruction.m_pMaterial );
//    this->m_sReconstructionFormat = _reconstruction.m_sReconstructionFormat;
//    this->m_sOrganName = _reconstruction.m_sOrganName;
//    this->m_sStructureType = _reconstruction.m_sStructureType;
//    this->m_bIsClosed = _reconstruction.m_bIsClosed;
//    this->m_bIsAutomatic = _reconstruction.m_bIsAutomatic;
//    this->m_dAvgVolume = _reconstruction.m_dAvgVolume;
//    this->m_dVolStdDeviation = _reconstruction.m_dVolStdDeviation;
//    this->m_dVolPctConfidence = _reconstruction.m_dVolPctConfidence;
//    this->m_sReconstructionTime = _reconstruction.m_sReconstructionTime;
//    this->m_bMaskGenerated = _reconstruction.m_bMaskGenerated;
//    this->m_ui8Level = _reconstruction.m_ui8Level;
//    this->m_ui16Label = _reconstruction.m_ui16Label;
//    this->m_bGenerated3D = _reconstruction.m_bGenerated3D;
//    this->m_sType3D = _reconstruction.m_sType3D;
//    this->m_fsPath = _reconstruction.m_fsPath;
//    this->m_i32DbID = _reconstruction.m_i32DbID;
//
//    ::fwData::Image::csptr img = _reconstruction.getImage();
//    if ( img != 0 )
//    {
//        this->setImage( ::boost::const_pointer_cast< ::fwData::Image > ( img ) );
//    }
//
//    ::fwData::TriangularMesh::csptr triangularMesh = _reconstruction.getTriangularMesh();
//    this->setTriangularMesh(::boost::const_pointer_cast< ::fwData::TriangularMesh >(triangularMesh));
//
//    return (*this);
//}

//------------------------------------------------------------------------------
//Reconstruction::sptr Reconstruction::clone() const
//{
//    SLM_TRACE("Reconstruction::clone()");
//    ::fwData::Reconstruction::NewSptr pNewReconstruction;
//
//    pNewReconstruction->m_bIsVisible = this->m_bIsVisible;
////  pNewReconstruction->m_dRed = this->m_dRed;
////  pNewReconstruction->m_dGreen = this->m_dGreen;
////  pNewReconstruction->m_dBlue = this->m_dBlue;
////  pNewReconstruction->m_dAlpha = this->m_dAlpha;
//    pNewReconstruction->setMaterial( this->m_pMaterial->clone() );
//    pNewReconstruction->m_sReconstructionFormat = this->m_sReconstructionFormat;
//    pNewReconstruction->m_sOrganName = this->m_sOrganName;
//    pNewReconstruction->m_sStructureType = this->m_sStructureType;
//    pNewReconstruction->m_bIsClosed = this->m_bIsClosed;
//    pNewReconstruction->m_bIsAutomatic = this->m_bIsAutomatic;
//    pNewReconstruction->m_dAvgVolume = this->m_dAvgVolume;
//    pNewReconstruction->m_dVolStdDeviation = this->m_dVolStdDeviation;
//    pNewReconstruction->m_dVolPctConfidence = this->m_dVolPctConfidence;
//    pNewReconstruction->m_sReconstructionTime = this->m_sReconstructionTime;
//    pNewReconstruction->m_bMaskGenerated = this->m_bMaskGenerated;
//    pNewReconstruction->m_ui8Level = this->m_ui8Level;
//    pNewReconstruction->m_ui16Label = this->m_ui16Label;
//    pNewReconstruction->m_bGenerated3D = this->m_bGenerated3D;
//    pNewReconstruction->m_sType3D = this->m_sType3D;
//    pNewReconstruction->m_fsPath = this->m_fsPath;
//    pNewReconstruction->m_i32DbID = this->m_i32DbID;
//
//    ::fwData::TriangularMesh::csptr triangularMesh = this->getTriangularMesh();
//    pNewReconstruction->setTriangularMesh(::boost::const_pointer_cast< ::fwData::TriangularMesh >(triangularMesh));
//
//    pNewReconstruction->setField(Reconstruction::ID_IMAGE);
//    ::fwData::Image::csptr img = this->getImage();
//    if ( img != 0 )
//    {
//        pNewReconstruction->setImage( img->clone() );
//    }
//
//    return pNewReconstruction;
//}

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
////------------------------------------------------------------------------------
//void Reconstruction::setIsVisible( const bool _bIsVisible )
//{
//  m_bIsVisible = _bIsVisible;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefIsVisible( const bool & _bIsVisible )
//{
//  m_bIsVisible = _bIsVisible;
//}
//
////------------------------------------------------------------------------------
//
//const bool Reconstruction::getIsVisible() const
//{
//  return m_bIsVisible;
//}
//
////------------------------------------------------------------------------------
//
//const bool & Reconstruction::getCRefIsVisible() const
//{
//  return m_bIsVisible;
//}
//
////------------------------------------------------------------------------------
//
//bool & Reconstruction::getRefIsVisible()
//{
//  return m_bIsVisible;
//}
//
//
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefReconstructionFormat( const std::string & _sReconstructionFormat )
//{
//  m_sReconstructionFormat = _sReconstructionFormat;
//}
//
////------------------------------------------------------------------------------
//
//const std::string Reconstruction::getReconstructionFormat() const
//{
//  return m_sReconstructionFormat;
//}
//
////------------------------------------------------------------------------------
//
//const std::string & Reconstruction::getCRefReconstructionFormat() const
//{
//  return m_sReconstructionFormat;
//}
//
////------------------------------------------------------------------------------
//
//std::string & Reconstruction::getRefReconstructionFormat()
//{
//  return m_sReconstructionFormat;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefOrganName( const std::string & _sOrganName )
//{
//  m_sOrganName = _sOrganName;
//}
//
////------------------------------------------------------------------------------
//
//const std::string Reconstruction::getOrganName() const
//{
//  return m_sOrganName;
//}
//
////------------------------------------------------------------------------------
//
//const std::string & Reconstruction::getCRefOrganName() const
//{
//  return m_sOrganName;
//}
//
////------------------------------------------------------------------------------
//
//std::string & Reconstruction::getRefOrganName()
//{
//  return m_sOrganName;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefStructureType( const std::string & _sStructureType )
//{
//  m_sStructureType = _sStructureType;
//}
//
////------------------------------------------------------------------------------
//
//const std::string Reconstruction::getStructureType() const
//{
//  return m_sStructureType;
//}
//
////------------------------------------------------------------------------------
//
//const std::string & Reconstruction::getCRefStructureType() const
//{
//  return m_sStructureType;
//}
//
////------------------------------------------------------------------------------
//
//std::string & Reconstruction::getRefStructureType()
//{
//  return m_sStructureType;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setIsClosed( const bool _bIsClosed )
//{
//  m_bIsClosed = _bIsClosed;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefIsClosed( const bool & _bIsClosed )
//{
//  m_bIsClosed = _bIsClosed;
//}
//
////------------------------------------------------------------------------------
//
//const bool Reconstruction::getIsClosed() const
//{
//  return m_bIsClosed;
//}
//
////------------------------------------------------------------------------------
//
//const bool & Reconstruction::getCRefIsClosed() const
//{
//  return m_bIsClosed;
//}
//
////------------------------------------------------------------------------------
//
//bool & Reconstruction::getRefIsClosed()
//{
//  return m_bIsClosed;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setIsAutomatic( const bool _bIsAutomatic )
//{
//  m_bIsAutomatic = _bIsAutomatic;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefIsAutomatic( const bool & _bIsAutomatic )
//{
//  m_bIsAutomatic = _bIsAutomatic;
//}
//
////------------------------------------------------------------------------------
//
//const bool Reconstruction::getIsAutomatic() const
//{
//  return m_bIsAutomatic;
//}
//
////------------------------------------------------------------------------------
//
//const bool & Reconstruction::getCRefIsAutomatic() const
//{
//  return m_bIsAutomatic;
//}
//
////------------------------------------------------------------------------------
//
//bool & Reconstruction::getRefIsAutomatic()
//{
//  return m_bIsAutomatic;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setAvgVolume( const double _dAvgVolume )
//{
//  m_dAvgVolume = _dAvgVolume;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefAvgVolume( const double & _dAvgVolume )
//{
//  m_dAvgVolume = _dAvgVolume;
//}
//
////------------------------------------------------------------------------------
//
//const double Reconstruction::getAvgVolume() const
//{
//  return m_dAvgVolume;
//}
//
////------------------------------------------------------------------------------
//
//const double & Reconstruction::getCRefAvgVolume() const
//{
//  return m_dAvgVolume;
//}
//
////------------------------------------------------------------------------------
//
//double & Reconstruction::getRefAvgVolume()
//{
//  return m_dAvgVolume;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setVolStdDeviation( const double _dVolStdDeviation )
//{
//  m_dVolStdDeviation = _dVolStdDeviation;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefVolStdDeviation( const double & _dVolStdDeviation )
//{
//  m_dVolStdDeviation = _dVolStdDeviation;
//}
//
////------------------------------------------------------------------------------
//
//const double Reconstruction::getVolStdDeviation() const
//{
//  return m_dVolStdDeviation;
//}
//
////------------------------------------------------------------------------------
//
//const double & Reconstruction::getCRefVolStdDeviation() const
//{
//  return m_dVolStdDeviation;
//}
//
////------------------------------------------------------------------------------
//
//double & Reconstruction::getRefVolStdDeviation()
//{
//  return m_dVolStdDeviation;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setVolPctConfidence( const double _dVolPctConfidence )
//{
//  m_dVolPctConfidence = _dVolPctConfidence;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefVolPctConfidence( const double & _dVolPctConfidence )
//{
//  m_dVolPctConfidence = _dVolPctConfidence;
//}
//
////------------------------------------------------------------------------------
//
//const double Reconstruction::getVolPctConfidence() const
//{
//  return m_dVolPctConfidence;
//}
//
////------------------------------------------------------------------------------
//
//const double & Reconstruction::getCRefVolPctConfidence() const
//{
//  return m_dVolPctConfidence;
//}
//
////------------------------------------------------------------------------------
//
//double & Reconstruction::getRefVolPctConfidence()
//{
//  return m_dVolPctConfidence;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefReconstructionTime( const std::string & _sReconstructionTime )
//{
//  m_sReconstructionTime = _sReconstructionTime;
//}
//
////------------------------------------------------------------------------------
//
//const std::string Reconstruction::getReconstructionTime() const
//{
//  return m_sReconstructionTime;
//}
//
////------------------------------------------------------------------------------
//
//const std::string & Reconstruction::getCRefReconstructionTime() const
//{
//  return m_sReconstructionTime;
//}
//
////------------------------------------------------------------------------------
//
//std::string & Reconstruction::getRefReconstructionTime()
//{
//  return m_sReconstructionTime;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setMaskGenerated( const bool _bMaskGenerated )
//{
//  m_bMaskGenerated = _bMaskGenerated;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefMaskGenerated( const bool & _bMaskGenerated )
//{
//  m_bMaskGenerated = _bMaskGenerated;
//}
//
////------------------------------------------------------------------------------
//
//const bool Reconstruction::getMaskGenerated() const
//{
//  return m_bMaskGenerated;
//}
//
////------------------------------------------------------------------------------
//
//const bool & Reconstruction::getCRefMaskGenerated() const
//{
//  return m_bMaskGenerated;
//}
//
////------------------------------------------------------------------------------
//
//bool & Reconstruction::getRefMaskGenerated()
//{
//  return m_bMaskGenerated;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setLevel( const boost::uint8_t  _ui8Level )
//{
//  m_ui8Level = _ui8Level;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefLevel( const boost::uint8_t  & _ui8Level )
//{
//  m_ui8Level = _ui8Level;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint8_t  Reconstruction::getLevel() const
//{
//  return m_ui8Level;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint8_t  & Reconstruction::getCRefLevel() const
//{
//  return m_ui8Level;
//}
//
////------------------------------------------------------------------------------
//
//boost::uint8_t  & Reconstruction::getRefLevel()
//{
//  return m_ui8Level;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setLabel( const boost::uint16_t  _ui16Label )
//{
//  m_ui16Label = _ui16Label;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefLabel( const boost::uint16_t  & _ui16Label )
//{
//  m_ui16Label = _ui16Label;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint16_t  Reconstruction::getLabel() const
//{
//  return m_ui16Label;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint16_t  & Reconstruction::getCRefLabel() const
//{
//  return m_ui16Label;
//}
//
////------------------------------------------------------------------------------
//
//boost::uint16_t  & Reconstruction::getRefLabel()
//{
//  return m_ui16Label;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setGenerated3D( const bool _bGenerated3D )
//{
//  m_bGenerated3D = _bGenerated3D;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefGenerated3D( const bool & _bGenerated3D )
//{
//  m_bGenerated3D = _bGenerated3D;
//}
//
////------------------------------------------------------------------------------
//
//const bool Reconstruction::getGenerated3D() const
//{
//  return m_bGenerated3D;
//}
//
////------------------------------------------------------------------------------
//
//const bool & Reconstruction::getCRefGenerated3D() const
//{
//  return m_bGenerated3D;
//}
//
////------------------------------------------------------------------------------
//
//bool & Reconstruction::getRefGenerated3D()
//{
//  return m_bGenerated3D;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefType3D( const std::string & _sType3D )
//{
//  m_sType3D = _sType3D;
//}
//
////------------------------------------------------------------------------------
//
//const std::string Reconstruction::getType3D() const
//{
//  return m_sType3D;
//}
//
////------------------------------------------------------------------------------
//
//const std::string & Reconstruction::getCRefType3D() const
//{
//  return m_sType3D;
//}
//
////------------------------------------------------------------------------------
//
//std::string & Reconstruction::getRefType3D()
//{
//  return m_sType3D;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefPath( const boost::filesystem::path & _fsPath )
//{
//  m_fsPath = _fsPath;
//}
//
////------------------------------------------------------------------------------
//
//const boost::filesystem::path Reconstruction::getPath() const
//{
//  return m_fsPath;
//}
//
////------------------------------------------------------------------------------
//
//const boost::filesystem::path & Reconstruction::getCRefPath() const
//{
//  return m_fsPath;
//}
//
////------------------------------------------------------------------------------
//
//boost::filesystem::path & Reconstruction::getRefPath()
//{
//  return m_fsPath;
//}


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
::fwData::TriangularMesh::csptr Reconstruction::getTriangularMesh() const
{
    return const_cast< Reconstruction *  >(this)->getTriangularMesh();
}


::fwData::TriangularMesh::sptr Reconstruction::getTriangularMesh()
{
    ::boost::shared_ptr< ::fwData::TriangularMesh> _pTriangularMesh;

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


void Reconstruction::setMaterial( ::fwData::Material::sptr _pMaterial )
{
    m_pMaterial = _pMaterial;
}
::fwData::Material::csptr Reconstruction::getMaterial() const
{
    return m_pMaterial;
}
::fwData::Material::sptr Reconstruction::getMaterial()
{
    return m_pMaterial;
}
;



////------------------------------------------------------------------------------
//
//void Reconstruction::setDbID( const boost::int32_t  _i32DbID )
//{
//  m_i32DbID = _i32DbID;
//}
//
////------------------------------------------------------------------------------
//
//void Reconstruction::setCRefDbID( const boost::int32_t  & _i32DbID )
//{
//  m_i32DbID = _i32DbID;
//}
//
////------------------------------------------------------------------------------
//
//const boost::int32_t  Reconstruction::getDbID() const
//{
//  return m_i32DbID;
//}
//
////------------------------------------------------------------------------------
//
//const boost::int32_t  & Reconstruction::getCRefDbID() const
//{
//  return m_i32DbID;
//}
//
////------------------------------------------------------------------------------
//
//boost::int32_t  & Reconstruction::getRefDbID()
//{
//  return m_i32DbID;
//}

//------------------------------------------------------------------------------


} // end namespace fwData
