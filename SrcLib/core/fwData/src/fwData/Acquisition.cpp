/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"

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
}

//------------------------------------------------------------------------------

Acquisition::~Acquisition()
{
}

//------------------------------------------------------------------------------

void Acquisition::shallowCopy( Acquisition::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_ui8BitsPerPixel = _source->m_ui8BitsPerPixel;
    m_fSliceThickness = _source->m_fSliceThickness;
    m_ui8Axe = _source->m_ui8Axe;
    m_bUnsignedFlag = _source->m_bUnsignedFlag;
    m_ui32AcquisitionIndex = _source->m_ui32AcquisitionIndex;
    m_sImageType = _source->m_sImageType;
    m_sImageFormat = _source->m_sImageFormat;
    m_ptCreationDate = _source->m_ptCreationDate;
    m_bIsMain = _source->m_bIsMain;
    m_bIsNormalDir = _source->m_bIsNormalDir;
    m_sUID = _source->m_sUID;
    m_i32DbID = _source->m_i32DbID;
    m_ui32LaboID = _source->m_ui32LaboID;
    m_ptDateSendToLaboAt = _source->m_ptDateSendToLaboAt;
    m_ptDateReceiveFromLaboAt = _source->m_ptDateReceiveFromLaboAt;
    m_ptDateSendToBDDAt = _source->m_ptDateSendToBDDAt;
    m_ptDateDisponibilityAt = _source->m_ptDateDisponibilityAt;
    m_dPatientSize = _source->m_dPatientSize;
    m_dPatientWeight = _source->m_dPatientWeight;
    m_dRadiations = _source->m_dRadiations;
    m_sMedicalPrinter = _source->m_sMedicalPrinter;
    m_sMedicalPrinterCorp = _source->m_sMedicalPrinterCorp;
    m_sPatientPosition = _source->m_sPatientPosition;
    m_dicomFileList = _source->m_dicomFileList;

    m_attrImage           = _source->m_attrImage;
    m_attrStructAnat      = _source->m_attrStructAnat;
    m_attrReconstructions = _source->m_attrReconstructions;
}

//------------------------------------------------------------------------------

void Acquisition::deepCopy( Acquisition::csptr _source )
{

    this->fieldDeepCopy( _source );
    m_ui8BitsPerPixel = _source->m_ui8BitsPerPixel;
    m_fSliceThickness = _source->m_fSliceThickness;
    m_ui8Axe = _source->m_ui8Axe;
    m_bUnsignedFlag = _source->m_bUnsignedFlag;
    m_ui32AcquisitionIndex = _source->m_ui32AcquisitionIndex;
    m_sImageType = _source->m_sImageType;
    m_sImageFormat = _source->m_sImageFormat;
    m_ptCreationDate = _source->m_ptCreationDate;
    m_bIsMain = _source->m_bIsMain;
    m_bIsNormalDir = _source->m_bIsNormalDir;
    m_sUID = _source->m_sUID;
    m_i32DbID = _source->m_i32DbID;
    m_ui32LaboID = _source->m_ui32LaboID;
    m_ptDateSendToLaboAt = _source->m_ptDateSendToLaboAt;
    m_ptDateReceiveFromLaboAt = _source->m_ptDateReceiveFromLaboAt;
    m_ptDateSendToBDDAt = _source->m_ptDateSendToBDDAt;
    m_ptDateDisponibilityAt = _source->m_ptDateDisponibilityAt;
    m_dPatientSize = _source->m_dPatientSize;
    m_dPatientWeight = _source->m_dPatientWeight;
    m_dRadiations = _source->m_dRadiations;
    m_sMedicalPrinter = _source->m_sMedicalPrinter;
    m_sMedicalPrinterCorp = _source->m_sMedicalPrinterCorp;
    m_sPatientPosition = _source->m_sPatientPosition;
    BOOST_FOREACH( std::string dicomFile , _source->m_dicomFileList)
    {
        m_dicomFileList.push_back(dicomFile);
    }

    m_attrImage           = ::fwData::Object::copy(_source->m_attrImage);
    m_attrStructAnat      = ::fwData::Object::copy(_source->m_attrStructAnat);
    m_attrReconstructions.clear();
    std::transform(
            _source->m_attrReconstructions.begin(), _source->m_attrReconstructions.end(),
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
