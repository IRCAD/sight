/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDataWriter.hxx"
#include "fwGdcmIO/writer/ie/FrameOfReference.hpp"

#include <fwMedData/Series.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

FrameOfReference::FrameOfReference(const SPTR(::gdcm::Writer)& writer,
                                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                   const ::fwMedData::Series::sptr& series,
                                   const ::fwLog::Logger::sptr& logger,
                                   ProgressCallback progress,
                                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >(writer, instance, series,
                                                                     logger, progress, cancel)
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
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Frame of Reference UID
    const std::string frameOfReferenceUID = m_instance->getFrameOfReferenceUID();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x0052 >(frameOfReferenceUID, dataset);

    // Position Reference Indicator - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setEmptyTagValue< 0x0020, 0x1040 >(dataset);

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
