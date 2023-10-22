/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/image.hpp>

namespace sight::io::dicom::reader::tid
{

/**
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class IO_DICOM_CLASS_API MeasurementReport : public io::dicom::reader::tid::TemplateID<data::image>
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
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::DicomInstance)& _instance,
        const data::image::sptr& _image,
        const core::log::logger::sptr& _logger
    );

    /// Destructor
    IO_DICOM_API ~MeasurementReport() override;

    /**
     * @brief Create Measurement Report template root node
     * @param[in] root Root node
     */
    IO_DICOM_API virtual void readSR(const SPTR(io::dicom::container::sr::DicomSRNode)& _root);
};

} // namespace sight::io::dicom::reader::tid
