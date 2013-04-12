/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Image.hpp"
#include "fwData/Reconstruction.hpp"

#include "fwData/Acquisition.hpp"

//------------------------------------------------------------------------------

fwDataRegisterMacro( ::fwData::Acquisition );

//------------------------------------------------------------------------------

namespace fwData
{

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

Acquisition::Acquisition( ::fwData::Object::Key key ) :
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
}

//------------------------------------------------------------------------------

Acquisition::~Acquisition()
{
}

//------------------------------------------------------------------------------

void Acquisition::shallowCopy(const Object::csptr &_source )
{
    Acquisition::csptr other = Acquisition::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldShallowCopy( _source );
    m_ui8BitsPerPixel = other->m_ui8BitsPerPixel;
    m_fSliceThickness = other->m_fSliceThickness;
    m_ui8Axe = other->m_ui8Axe;
    m_bUnsignedFlag = other->m_bUnsignedFlag;
    m_ui32AcquisitionIndex = other->m_ui32AcquisitionIndex;
    m_sImageType = other->m_sImageType;
    m_sImageFormat = other->m_sImageFormat;
    m_ptCreationDate = other->m_ptCreationDate;
    m_bIsMain = other->m_bIsMain;
    m_bIsNormalDir = other->m_bIsNormalDir;
    m_sUID = other->m_sUID;
    m_i32DbID = other->m_i32DbID;
    m_ui32LaboID = other->m_ui32LaboID;
    m_ui32NetID = other->m_ui32NetID;
    m_ptDateSendToLaboAt = other->m_ptDateSendToLaboAt;
    m_ptDateReceiveFromLaboAt = other->m_ptDateReceiveFromLaboAt;
    m_ptDateSendToBDDAt = other->m_ptDateSendToBDDAt;
    m_ptDateDisponibilityAt = other->m_ptDateDisponibilityAt;
    m_dPatientSize = other->m_dPatientSize;
    m_dPatientWeight = other->m_dPatientWeight;
    m_dRadiations = other->m_dRadiations;
    m_sMedicalPrinter = other->m_sMedicalPrinter;
    m_sMedicalPrinterCorp = other->m_sMedicalPrinterCorp;
    m_sPatientPosition = other->m_sPatientPosition;
    m_dicomFileList = other->m_dicomFileList;

    m_attrImage           = other->m_attrImage;
    m_attrStructAnat      = other->m_attrStructAnat;
    m_attrReconstructions = other->m_attrReconstructions;
}

//------------------------------------------------------------------------------
void Acquisition::deepCopy(const Object::csptr &_source )
{
    Acquisition::csptr other = Acquisition::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );

    this->fieldDeepCopy( _source );
    m_ui8BitsPerPixel = other->m_ui8BitsPerPixel;
    m_fSliceThickness = other->m_fSliceThickness;
    m_ui8Axe = other->m_ui8Axe;
    m_bUnsignedFlag = other->m_bUnsignedFlag;
    m_ui32AcquisitionIndex = other->m_ui32AcquisitionIndex;
    m_sImageType = other->m_sImageType;
    m_sImageFormat = other->m_sImageFormat;
    m_ptCreationDate = other->m_ptCreationDate;
    m_bIsMain = other->m_bIsMain;
    m_bIsNormalDir = other->m_bIsNormalDir;
    m_sUID = other->m_sUID;
    m_i32DbID = other->m_i32DbID;
    m_ui32LaboID = other->m_ui32LaboID;
    m_ui32NetID = other->m_ui32NetID;
    m_ptDateSendToLaboAt = other->m_ptDateSendToLaboAt;
    m_ptDateReceiveFromLaboAt = other->m_ptDateReceiveFromLaboAt;
    m_ptDateSendToBDDAt = other->m_ptDateSendToBDDAt;
    m_ptDateDisponibilityAt = other->m_ptDateDisponibilityAt;
    m_dPatientSize = other->m_dPatientSize;
    m_dPatientWeight = other->m_dPatientWeight;
    m_dRadiations = other->m_dRadiations;
    m_sMedicalPrinter = other->m_sMedicalPrinter;
    m_sMedicalPrinterCorp = other->m_sMedicalPrinterCorp;
    m_sPatientPosition = other->m_sPatientPosition;
    BOOST_FOREACH( std::string dicomFile , other->m_dicomFileList)
    {
        m_dicomFileList.push_back(dicomFile);
    }

    m_attrImage           = ::fwData::Object::copy(other->m_attrImage);
    m_attrStructAnat      = ::fwData::Object::copy(other->m_attrStructAnat);
    m_attrReconstructions.clear();
    std::transform(
            other->m_attrReconstructions.begin(), other->m_attrReconstructions.end(),
            std::back_inserter(m_attrReconstructions),
            & ::fwData::Object::copy< ReconstructionContainerType::value_type::element_type >
            );
}
//------------------------------------------------------------------------------

Acquisition::ReconstructionContainerType::size_type Acquisition::getNumberOfReconstructions() const
{
    return this->getReconstructions().size();
}

//------------------------------------------------------------------------------

void Acquisition::addReconstruction( ::fwData::Reconstruction::sptr _reconstruction )
{
    m_attrReconstructions.push_back(_reconstruction);
}

//------------------------------------------------------------------------------

void Acquisition::addDicomFileUrl(std::string dicomfileUrl)
{
    m_dicomFileList.push_back(dicomfileUrl);
}

//------------------------------------------------------------------------------

} // end namespace fwData
