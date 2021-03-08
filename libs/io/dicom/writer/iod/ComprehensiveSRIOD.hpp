/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomInstance.hpp"
#include "io/dicom/writer/iod/InformationObjectDefinition.hpp"

namespace sight::io::dicom
{
namespace writer
{
namespace iod
{

/**
 * @brief ComprehensiveSRIOD class used to write Enhanced Structured Report DICOM files
 */
class IO_DICOM_CLASS_API ComprehensiveSRIOD : public io::dicom::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] destinationPath Destination path
     * @param[in] use3DSR Use 3D Structural Report IOD
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API ComprehensiveSRIOD(const SPTR(io::dicom::container::DicomInstance)& instance,
                                    const std::filesystem::path& destinationPath,
                                    bool use3DSR                          = false,
                                    const core::log::Logger::sptr& logger = nullptr,
                                    ProgressCallback progress             = nullptr,
                                    CancelRequestedCallback cancel        = nullptr);

    /// Destructor
    IO_DICOM_API virtual ~ComprehensiveSRIOD();

    /// Write DICOM file
    IO_DICOM_API void write(const data::Series::csptr& series) override;

protected:
    /// True if we must use 3DSR
    bool m_use3DSR;

};

} // namespace iod
} // namespace writer
} // namespace sight::io::dicom
