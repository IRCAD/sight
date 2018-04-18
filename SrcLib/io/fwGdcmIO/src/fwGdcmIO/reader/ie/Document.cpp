/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/reader/ie/Document.hpp"

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "fwGdcmIO/helper/StructuredReport.hpp"
#include "fwGdcmIO/reader/tid/MeasurementReport.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Document::Document(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                   const SPTR(::gdcm::Reader)& reader,
                   const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                   const ::fwData::Image::sptr& image,
                   const ::fwLog::Logger::sptr& logger,
                   ProgressCallback progress,
                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image,
                                                                 logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::readSR() throw (::fwGdcmIO::exception::Failed)
{
    // Retrieve dataset
    const ::gdcm::DataSet& datasetRoot = m_reader->GetFile().GetDataSet();

    // Create SR from Dataset
    SPTR(::fwGdcmIO::container::sr::DicomSRContainerNode) rootContainerNode =
        ::fwGdcmIO::helper::StructuredReport::readSR(datasetRoot);

    if(rootContainerNode)
    {
        // Try to read a measurement report
        ::fwGdcmIO::reader::tid::MeasurementReport report(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
        report.readSR(rootContainerNode);
    }

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
