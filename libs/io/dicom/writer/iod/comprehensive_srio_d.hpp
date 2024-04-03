/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/writer/iod/information_object_definition.hpp"

namespace sight::io::dicom::writer::iod
{

/**
 * @brief ComprehensiveSRIOD class used to write Enhanced Structured Report DICOM files
 */
class SIGHT_IO_DICOM_CLASS_API comprehensive_sriod : public io::dicom::writer::iod::information_object_definition
{
public:

    /**
     * @brief Constructor
     * @param[in] _instance DICOM instance used to share informations between modules
     * @param[in] _destination_path Destination path
     * @param[in] _use_3d_sr Use 3D Structural Report IOD
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API comprehensive_sriod(
        const SPTR(io::dicom::container::dicom_instance)& _instance,
        const std::filesystem::path& _destination_path,
        bool _use_3d_sr                        = false,
        const core::log::logger::sptr& _logger = nullptr,
        progress_callback _progress            = nullptr,
        cancel_requested_callback _cancel      = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API ~comprehensive_sriod() override;

    /// Write DICOM file
    SIGHT_IO_DICOM_API void write(const data::series::csptr& _series) override;

protected:

    /// True if we must use 3DSR
    bool m_use_3d_sr;
};

} // namespace sight::io::dicom::writer::iod
