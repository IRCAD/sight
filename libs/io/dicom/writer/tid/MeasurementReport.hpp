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
#include "io/dicom/writer/tid/TemplateID.hpp"

#include <data/Image.hpp>

namespace sight::io::dicom
{

namespace writer
{

namespace tid
{

/**
 * @brief Measurement Report TID - This a root template
 * @see TID t1tt1 FIXME: Set the final TID
 */
class IO_DICOM_CLASS_API MeasurementReport : public io::dicom::writer::tid::TemplateID<data::Image>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] image Image data
     */
    IO_DICOM_API MeasurementReport(
        const SPTR(::gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::Image::csptr& image
    );

    /// Destructor
    IO_DICOM_API virtual ~MeasurementReport();

    /**
     * @brief Create Measurement Report template root node
     * @param[in] useSCoord3D True if we must write SCOORD3D, false if we must write SCOORD
     */
    IO_DICOM_API virtual SPTR(io::dicom::container::sr::DicomSRNode) createRootNode(bool useSCoord3D = true);
};

} // namespace tid

} // namespace writer

} // namespace sight::io::dicom
