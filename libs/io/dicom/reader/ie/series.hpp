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

#include "io/dicom/reader/ie/information_entity.hpp"

#include <data/series.hpp>

namespace sight::io::dicom::reader::ie
{

/**
 * @brief series Information Entity class
 */
class IO_DICOM_CLASS_API series : public io::dicom::reader::ie::information_entity<data::series>
{
public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API series(
        const CSPTR(data::dicom_series)& _dicom_series,
        const SPTR(gdcm::Reader)& _reader,
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const data::series::sptr& _series,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    IO_DICOM_API ~series() override;

    /**
     * @brief Read General series Module tags
     * @see PS 3.3 C.7.3.1
     */
    IO_DICOM_API virtual void read_general_series_module();
};

} // namespace sight::io::dicom::reader::ie
