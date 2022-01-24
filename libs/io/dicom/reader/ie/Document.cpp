/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/reader/ie/Document.hpp"

#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRContainerNode.hpp"
#include "io/dicom/helper/StructuredReport.hpp"
#include "io/dicom/reader/tid/MeasurementReport.hpp"

namespace sight::io::dicom
{

namespace reader
{

namespace ie
{

//------------------------------------------------------------------------------

Document::Document(
    const data::DicomSeries::csptr& dicomSeries,
    const SPTR(gdcm::Reader)& reader,
    const io::dicom::container::DicomInstance::sptr& instance,
    const data::Image::sptr& image,
    const core::log::Logger::sptr& logger,
    ProgressCallback progress,
    CancelRequestedCallback cancel
) :
    io::dicom::reader::ie::InformationEntity<data::Image>(dicomSeries, reader, instance, image,
                                                          logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Document::~Document()
{
}

//------------------------------------------------------------------------------

void Document::readSR()
{
    // Retrieve dataset
    const gdcm::DataSet& datasetRoot = m_reader->GetFile().GetDataSet();

    // Create SR from Dataset
    SPTR(io::dicom::container::sr::DicomSRContainerNode) rootContainerNode =
        io::dicom::helper::StructuredReport::readSR(datasetRoot);

    if(rootContainerNode)
    {
        // Try to read a measurement report
        io::dicom::reader::tid::MeasurementReport report(m_dicomSeries, m_reader, m_instance, m_object, m_logger);
        report.readSR(rootContainerNode);
    }
}

//------------------------------------------------------------------------------

} // namespace ie

} // namespace reader

} // namespace sight::io::dicom
