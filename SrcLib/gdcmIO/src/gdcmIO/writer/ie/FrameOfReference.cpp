/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmUIDGenerator.h>

#include <fwMedData/Series.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/writer/ie/FrameOfReference.hpp"

namespace gdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

FrameOfReference::FrameOfReference(SPTR(::gdcm::Writer) writer,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwMedData::Series::sptr series):
        ::gdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >(writer, instance, series)
{
}

//------------------------------------------------------------------------------

FrameOfReference::~FrameOfReference()
{
}

//------------------------------------------------------------------------------

void FrameOfReference::writeFrameOfReferenceModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_writer->GetFile().GetDataSet();

    // Frame of Reference UID
    const std::string frameOfReferenceUID = m_instance->getCRefSOPInstanceUIDContainer()[0];
    ::gdcmIO::helper::DicomData::setTagValue< 0x0020, 0x0052 >(frameOfReferenceUID, dataset);

    // Position Reference Indicator - Type 2
    ::gdcm::Attribute< 0x0020, 0x1040 > positionReferenceIndicatorAttribute;
    dataset.Insert(positionReferenceIndicatorAttribute.GetAsDataElement());

}

} // namespace ie
} // namespace writer
} // namespace gdcmIO
