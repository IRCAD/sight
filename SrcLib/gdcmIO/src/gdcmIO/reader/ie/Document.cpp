/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>

#include "gdcmIO/container/DicomCodedAttribute.hpp"
#include "gdcmIO/container/sr/DicomSRContainerNode.hpp"
#include "gdcmIO/helper/StructuredReport.hpp"
#include "gdcmIO/reader/tid/MeasurementReport.hpp"

#include "gdcmIO/reader/ie/Document.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Document::Document(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                   SPTR(::gdcm::Reader)reader,
                   SPTR(::gdcmIO::container::DicomInstance)instance,
                   ::fwData::Image::sptr image) :
    ::gdcmIO::reader::ie::InformationEntity< ::fwData::Image >(dicomSeries, reader, instance, image)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::readSR() throw (::gdcmIO::exception::Failed)
{
    // Retrieve dataset
    const ::gdcm::DataSet& datasetRoot = m_reader->GetFile().GetDataSet();

    // Create SR from Dataset
    SPTR(::gdcmIO::container::sr::DicomSRContainerNode) rootContainerNode =
        ::gdcmIO::helper::StructuredReport::readSR(datasetRoot);

    if(rootContainerNode)
    {
        // Try to read a measurement report
        ::gdcmIO::reader::tid::MeasurementReport report(m_dicomSeries, m_reader, m_instance, m_object);
        report.readSR(rootContainerNode);
    }

}

//------------------------------------------------------------------------------


} // namespace ie
} // namespace reader
} // namespace gdcmIO
