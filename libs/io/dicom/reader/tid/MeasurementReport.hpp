/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "io/dicom/container/sr/DicomSRNode.hpp"
#include "io/dicom/reader/tid/TemplateID.hpp"

#include <data/Image.hpp>

namespace sight::io::dicom
{

namespace reader
{

namespace tid
{

/**
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class IO_DICOM_CLASS_API MeasurementReport : public io::dicom::reader::tid::TemplateID<data::Image>
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Sight data object
     * @param[in] logger Logger
     */
    IO_DICOM_API MeasurementReport(
        const CSPTR(data::DicomSeries)& dicomSeries,
        const SPTR(::gdcm::Reader)& reader,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Image::sptr& image,
        const core::log::Logger::sptr& logger
    );

    /// Destructor
    IO_DICOM_API virtual ~MeasurementReport();

    /**
     * @brief Create Measurement Report template root node
     * @param[in] root Root node
     */
    IO_DICOM_API virtual void readSR(const SPTR(io::dicom::container::sr::DicomSRNode)& root);
};

} // namespace tid

} // namespace reader

} // namespace sight::io::dicom
