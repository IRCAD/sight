/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "io/dicom/exception/Failed.hpp"
#include "io/dicom/writer/ie/InformationEntity.hpp"

#include <data/image.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Spatial Fiducials Information Entity class
 */
class IO_DICOM_CLASS_API spatial_fiducials : public io::dicom::writer::ie::information_entity<data::image>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API spatial_fiducials(
        const SPTR(gdcm::Writer)& _writer,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::image::csptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    IO_DICOM_API ~spatial_fiducials() override;

    /**
     * @brief Write Spatial Fiducials Module tags
     * @see PS 3.3 C.21.2
     */
    IO_DICOM_API virtual void write_spatial_fiducials_module();

    /**
     * @brief Write Common Instance Reference Module tags
     * @see PS 3.3 C.12.2
     */
    IO_DICOM_API virtual void write_common_instance_reference_module();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    IO_DICOM_API virtual void write_sop_common_module();

protected:

    /**
     * @brief Write image landmarks into sequence
     * @throw(io::dicom::exception::Failed)
     */
    void write_landmarks(gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence);

    /**
     * @brief Write image distances into sequence
     */
    void write_distances(gdcm::SmartPointer<gdcm::SequenceOfItems> _sequence);

    /**
     * @brief Add a frame number to the referenced image sequence
     * @param[in] frameNumber Frame Number
     * @param[in] referencedImageSequence Destination sequence
     * @throw(io::dicom::exception::Failed)
     */
    void add_referenced_image(
        int _frame_number,
        gdcm::SmartPointer<gdcm::SequenceOfItems> _referenced_image_sequence
    );
};

} // namespace sight::io::dicom::writer::ie
