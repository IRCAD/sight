/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Image.hpp"
#include "fwData/Pipeline.hpp"
#include "fwData/Reconstruction.hpp"

#include <fwTools/Factory.hpp>

#include "fwData/Acquisition.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Acquisition, ::fwData::Acquisition);
namespace fwData
{
//------------------------------------------------------------------------------

const Object::FieldID Acquisition::ID_RECONSTRUCTIONS = "ID_RECONSTRUCTIONS";
const Object::FieldID Acquisition::ID_IMAGE = "ID_IMAGE";
const Object::FieldID Acquisition::ID_STRUCTANAT = "ID_STRUCTANAT";

//------------------------------------------------------------------------------

Acquisition::Acquisition() :
    m_ui8BitsPerPixel(0),
    m_fSliceThickness(0),
    m_ui8Axe(0),
    m_bUnsignedFlag(false),
    m_ui32AcquisitionIndex(0),
    m_sImageType(""),
    m_sImageFormat(""),
    m_ptCreationDate ( ::boost::date_time::min_date_time ),
    m_bIsMain(true),
    m_bIsNormalDir(true),
    m_sUID(""),
    m_i32DbID(-1),
    m_ui32LaboID(0),
    m_ui32NetID(0),
    m_ptDateSendToLaboAt ( ::boost::date_time::min_date_time ),
    m_ptDateReceiveFromLaboAt ( ::boost::date_time::min_date_time ),
    m_ptDateSendToBDDAt ( ::boost::date_time::min_date_time ),
    m_ptDateDisponibilityAt ( ::boost::date_time::min_date_time ),
    m_dPatientSize(0),
    m_dPatientWeight(0),
    m_dRadiations(0),
    m_sMedicalPrinter(""),
    m_sMedicalPrinterCorp(""),
    m_sPatientPosition("")
{
    SLM_WARN("::fwData::Acquisition() : (ToDo) field default value");
    setField( Acquisition::ID_RECONSTRUCTIONS );
    setField( Acquisition::ID_IMAGE);
    setField( Acquisition::ID_STRUCTANAT );
}

//------------------------------------------------------------------------------

Acquisition::~Acquisition()
{
    SLM_WARN("::fwData::~Acquisition() : (ToDo)");
}

//------------------------------------------------------------------------------

Acquisition &Acquisition::operator=(const Acquisition & _acquisition)
{
    this->m_ui8BitsPerPixel = _acquisition.m_ui8BitsPerPixel;
    this->m_fSliceThickness = _acquisition.m_fSliceThickness;
    this->m_ui8Axe = _acquisition.m_ui8Axe;
    this->m_bUnsignedFlag = _acquisition.m_bUnsignedFlag;
    this->m_ui32AcquisitionIndex = _acquisition.m_ui32AcquisitionIndex;
    this->m_sImageType = _acquisition.m_sImageType;
    this->m_sImageFormat = _acquisition.m_sImageFormat;
    this->m_ptCreationDate = _acquisition.m_ptCreationDate;
    this->m_bIsMain = _acquisition.m_bIsMain;
    this->m_bIsNormalDir = _acquisition.m_bIsNormalDir;
    this->m_sUID = _acquisition.m_sUID;
    this->m_i32DbID = _acquisition.m_i32DbID;
    this->m_ui32LaboID = _acquisition.m_ui32LaboID;
    this->m_ptDateSendToLaboAt = _acquisition.m_ptDateSendToLaboAt;
    this->m_ptDateReceiveFromLaboAt = _acquisition.m_ptDateReceiveFromLaboAt;
    this->m_ptDateSendToBDDAt = _acquisition.m_ptDateSendToBDDAt;
    this->m_ptDateDisponibilityAt = _acquisition.m_ptDateDisponibilityAt;
    this->m_dPatientSize = _acquisition.m_dPatientSize;
    this->m_dPatientWeight = _acquisition.m_dPatientWeight;
    this->m_dRadiations = _acquisition.m_dRadiations;
    this->m_sMedicalPrinter = _acquisition.m_sMedicalPrinter;
    this->m_sMedicalPrinterCorp = _acquisition.m_sMedicalPrinterCorp;
    this->m_sPatientPosition = _acquisition.m_sPatientPosition;

    ::fwData::Image::csptr img = _acquisition.getImage();
    if ( img != 0 )
    {
        this->setImage( ::boost::const_pointer_cast< ::fwData::Image >( img ) );
    }

    img = _acquisition.getStructAnat();
    if ( img != 0 )
    {
        this->setStructAnat( ::boost::const_pointer_cast< ::fwData::Image >( img ) );
    }

    //this->setImage( ::boost::const_pointer_cast< ::fwData::Image >(_acquisition.getImage()));

    this->getField( Acquisition::ID_RECONSTRUCTIONS )->children() = _acquisition.getField( Acquisition::ID_RECONSTRUCTIONS )->children();

    return (*this);
}

//------------------------------------------------------------------------------

Acquisition::sptr Acquisition::clone() const
{
    SLM_TRACE("Acquisition::clone()");
    ::fwData::Acquisition::NewSptr pNewAcquisition;

    pNewAcquisition->m_ui8BitsPerPixel = this->m_ui8BitsPerPixel;
    pNewAcquisition->m_fSliceThickness = this->m_fSliceThickness;
    pNewAcquisition->m_ui8Axe = this->m_ui8Axe;
    pNewAcquisition->m_bUnsignedFlag = this->m_bUnsignedFlag;
    pNewAcquisition->m_ui32AcquisitionIndex = this->m_ui32AcquisitionIndex;
    pNewAcquisition->m_sImageType = this->m_sImageType;
    pNewAcquisition->m_sImageFormat = this->m_sImageFormat;
    pNewAcquisition->m_ptCreationDate  = this->m_ptCreationDate;
    pNewAcquisition->m_bIsMain = this->m_bIsMain;
    pNewAcquisition->m_bIsNormalDir = this->m_bIsNormalDir;
    pNewAcquisition->m_sUID = this->m_sUID;
    pNewAcquisition->m_i32DbID = this->m_i32DbID;
    pNewAcquisition->m_ui32LaboID = this->m_ui32LaboID;
    pNewAcquisition->m_ui32NetID = this->m_ui32NetID;
    pNewAcquisition->m_ptDateReceiveFromLaboAt = this->m_ptDateReceiveFromLaboAt;
    pNewAcquisition->m_ptDateSendToBDDAt = this->m_ptDateSendToBDDAt;
    pNewAcquisition->m_ptDateDisponibilityAt = this->m_ptDateDisponibilityAt;
    pNewAcquisition->m_dPatientSize = this->m_dPatientSize;
    pNewAcquisition->m_dPatientWeight = this->m_dPatientWeight;
    pNewAcquisition->m_dRadiations = this->m_dRadiations;
    pNewAcquisition->m_sMedicalPrinter = this->m_sMedicalPrinter;
    pNewAcquisition->m_sMedicalPrinterCorp = this->m_sMedicalPrinterCorp;
    pNewAcquisition->m_sPatientPosition = this->m_sPatientPosition;

    ::fwData::Image::csptr img = this->getImage();
    if ( img != 0 )
    {
        pNewAcquisition->setImage( img->clone() );
    }

    img = this->getStructAnat();
    if ( img != 0 )
    {
        pNewAcquisition->setStructAnat( img->clone() );
    }

    ::fwData::Acquisition::ReconstructionConstIterator reconstructionIter = this->getReconstructions().first;
    ::fwData::Acquisition::ReconstructionConstIterator reconstructionIterEnd = this->getReconstructions().second;
    while ( reconstructionIter != reconstructionIterEnd )
    {
        pNewAcquisition->addReconstruction( (*reconstructionIter)->clone() );
        reconstructionIter++;
    }

    return pNewAcquisition;
}

//------------------------------------------------------------------------------

void Acquisition::setImage( ::fwData::Image::sptr _pImage )
{
    if ( _pImage == 0 )
    {
        SLM_WARN("Acquisition::setImage : the image pointer is null.");
        this->setField( Acquisition::ID_IMAGE ); // the previous image is deleted
    }
    else
    {
        this->setFieldSingleElement( Acquisition::ID_IMAGE, _pImage );
    }
}

//------------------------------------------------------------------------------

::fwData::Image::csptr Acquisition::getImage() const
{
    ::fwData::Image::csptr _pImage;

    const unsigned int NbChildren = this->getField( Acquisition::ID_IMAGE )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
    {
        _pImage = ::fwData::Image::dynamicCast ( this->getField( Acquisition::ID_IMAGE )->children()[0] );
    }
    else
    {
        SLM_WARN("Acquisition::getImage : return an image pointer is null.");
    }

    return _pImage;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr Acquisition::getImage()
{
    ::fwData::Image::sptr _pImage;

    const unsigned int NbChildren = this->getField( Acquisition::ID_IMAGE )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
    {
        _pImage = ::fwData::Image::dynamicCast ( this->getField( Acquisition::ID_IMAGE )->children()[0] );
    }
    else
    {
        SLM_WARN("Acquisition::getImage : return an image pointer is null.");
    }

    return _pImage;
}

//------------------------------------------------------------------------------

void Acquisition::setStructAnat( ::fwData::Image::sptr _pStructAnat )
{
    if ( _pStructAnat == 0 )
    {
        SLM_WARN("Acquisition::setStructAnat : the image pointer is null.");
        this->setField( Acquisition::ID_STRUCTANAT ); // the previous image is deleted
    }
    else
    {
        this->setFieldSingleElement( Acquisition::ID_STRUCTANAT, _pStructAnat );
    }
}

//------------------------------------------------------------------------------

::fwData::Image::csptr Acquisition::getStructAnat() const
{
    ::fwData::Image::csptr _pStructAnat;

    const unsigned int NbChildren = this->getField( Acquisition::ID_STRUCTANAT )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
    {
        _pStructAnat = ::fwData::Image::dynamicCast ( this->getField( Acquisition::ID_STRUCTANAT )->children()[0] );
    }
    else
    {
        SLM_WARN("Acquisition::getStructAnat : return an image pointer is null.");
    }

    return _pStructAnat;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr Acquisition::getStructAnat()
{
    ::fwData::Image::sptr _pStructAnat;

    const unsigned int NbChildren = this->getField( Acquisition::ID_STRUCTANAT )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the image is allocated
    if( NbChildren == 1 )
    {
        _pStructAnat = ::fwData::Image::dynamicCast ( this->getField( Acquisition::ID_STRUCTANAT )->children()[0] );
    }
    else
    {
        SLM_WARN("Acquisition::getStructAnat : return an image pointer is null.");
    }

    return _pStructAnat;
}

//------------------------------------------------------------------------------

boost::uint32_t  Acquisition::getReconstructionSize() const
{
    return this->getField( Acquisition::ID_RECONSTRUCTIONS )->children().size();
}

//------------------------------------------------------------------------------

void Acquisition::addReconstruction( ::fwData::Reconstruction::sptr _reconstruction )
{
    this->addFieldElement( Acquisition::ID_RECONSTRUCTIONS, _reconstruction );
}

//------------------------------------------------------------------------------

std::pair< Acquisition::ReconstructionIterator, Acquisition::ReconstructionIterator > Acquisition::getReconstructions()
{
    ReconstructionIterator begin(  getField( Acquisition::ID_RECONSTRUCTIONS )->children().begin() );
    ReconstructionIterator   end(  getField( Acquisition::ID_RECONSTRUCTIONS )->children().end()   );
    return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< Acquisition::ReconstructionConstIterator, Acquisition::ReconstructionConstIterator > Acquisition::getReconstructions() const
{
    ReconstructionConstIterator begin(  getField( Acquisition::ID_RECONSTRUCTIONS )->children().begin()   );
    ReconstructionConstIterator   end(  getField( Acquisition::ID_RECONSTRUCTIONS )->children().end()   );
    return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

} // end namespace fwData
