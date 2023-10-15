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

#include "io/dicom/writer/ie/InformationEntity.hpp"

#include <data/series.hpp>

namespace sight::io::dicom::writer::ie
{

/**
 * @brief Equipment Information Entity class
 */
class IO_DICOM_CLASS_API Equipment : public io::dicom::writer::ie::InformationEntity<data::series>
{
public:

    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Equipment data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API Equipment(
        const SPTR(gdcm::Writer)& writer,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const data::series::csptr& series,
        const core::log::logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API ~Equipment() override;

    /**
     * @brief Write General Equipment Module tags
     * @see PS 3.3 C.7.5.1
     */
    IO_DICOM_API virtual void writeGeneralEquipmentModule();

    /**
     * @brief Write Enhanced General Equipment Module tags
     * @see PS 3.3 C.7.5.2
     */
    IO_DICOM_API virtual void writeEnhancedGeneralEquipmentModule();
};

} // namespace sight::io::dicom::writer::ie
