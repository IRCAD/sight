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
    m_sPatientPosition(""),
    m_pathToFiles("")
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

void Acquisition::shallowCopy( Acquisition::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_ui8BitsPerPixel = _source->m_ui8BitsPerPixel;
    this->m_fSliceThickness = _source->m_fSliceThickness;
    this->m_ui8Axe = _source->m_ui8Axe;
    this->m_bUnsignedFlag = _source->m_bUnsignedFlag;
    this->m_ui32AcquisitionIndex = _source->m_ui32AcquisitionIndex;
    this->m_sImageType = _source->m_sImageType;
    this->m_sImageFormat = _source->m_sImageFormat;
    this->m_ptCreationDate = _source->m_ptCreationDate;
    this->m_bIsMain = _source->m_bIsMain;
    this->m_bIsNormalDir = _source->m_bIsNormalDir;
    this->m_sUID = _source->m_sUID;
    this->m_i32DbID = _source->m_i32DbID;
    this->m_ui32LaboID = _source->m_ui32LaboID;
    this->m_ptDateSendToLaboAt = _source->m_ptDateSendToLaboAt;
    this->m_ptDateReceiveFromLaboAt = _source->m_ptDateReceiveFromLaboAt;
    this->m_ptDateSendToBDDAt = _source->m_ptDateSendToBDDAt;
    this->m_ptDateDisponibilityAt = _source->m_ptDateDisponibilityAt;
    this->m_dPatientSize = _source->m_dPatientSize;
    this->m_dPatientWeight = _source->m_dPatientWeight;
    this->m_dRadiations = _source->m_dRadiations;
    this->m_sMedicalPrinter = _source->m_sMedicalPrinter;
    this->m_sMedicalPrinterCorp = _source->m_sMedicalPrinterCorp;
    this->m_sPatientPosition = _source->m_sPatientPosition;
    this->m_dicomFileList = _source->m_dicomFileList;

}

//------------------------------------------------------------------------------

void Acquisition::deepCopy( Acquisition::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_ui8BitsPerPixel = _source->m_ui8BitsPerPixel;
    this->m_fSliceThickness = _source->m_fSliceThickness;
    this->m_ui8Axe = _source->m_ui8Axe;
    this->m_bUnsignedFlag = _source->m_bUnsignedFlag;
    this->m_ui32AcquisitionIndex = _source->m_ui32AcquisitionIndex;
    this->m_sImageType = _source->m_sImageType;
    this->m_sImageFormat = _source->m_sImageFormat;
    this->m_ptCreationDate = _source->m_ptCreationDate;
    this->m_bIsMain = _source->m_bIsMain;
    this->m_bIsNormalDir = _source->m_bIsNormalDir;
    this->m_sUID = _source->m_sUID;
    this->m_i32DbID = _source->m_i32DbID;
    this->m_ui32LaboID = _source->m_ui32LaboID;
    this->m_ptDateSendToLaboAt = _source->m_ptDateSendToLaboAt;
    this->m_ptDateReceiveFromLaboAt = _source->m_ptDateReceiveFromLaboAt;
    this->m_ptDateSendToBDDAt = _source->m_ptDateSendToBDDAt;
    this->m_ptDateDisponibilityAt = _source->m_ptDateDisponibilityAt;
    this->m_dPatientSize = _source->m_dPatientSize;
    this->m_dPatientWeight = _source->m_dPatientWeight;
    this->m_dRadiations = _source->m_dRadiations;
    this->m_sMedicalPrinter = _source->m_sMedicalPrinter;
    this->m_sMedicalPrinterCorp = _source->m_sMedicalPrinterCorp;
    this->m_sPatientPosition = _source->m_sPatientPosition;
    BOOST_FOREACH( std::string dicomFile , _source->m_dicomFileList)
    {
        this->m_dicomFileList.push_back(dicomFile);
    }
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
        SLM_WARN("Acquisition::getImage : return a null pointer.");
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
        SLM_WARN("Acquisition::getImage : return a null pointer.");
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
        SLM_WARN("Acquisition::getStructAnat : return a null pointer.");
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
        SLM_WARN("Acquisition::getStructAnat : return a null pointer.");
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

void Acquisition::addDicomFileUrl(std::string dicomfileUrl)
{
    m_dicomFileList.push_back(dicomfileUrl);
}

//------------------------------------------------------------------------------

} // end namespace fwData
