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

#include "io/dicom/config.hpp"
#include "io/dicom/writer/Series.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_adviser.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/__/writer/GenericObjectWriter.hpp>

namespace sight::io::dicom
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 */
class IO_DICOM_CLASS_API SeriesSet : public io::writer::GenericObjectWriter<data::SeriesSet>,
                                     public core::location::single_folder,
                                     public core::tools::progress_adviser
{
public:

    SIGHT_DECLARE_CLASS(
        SeriesSet,
        io::writer::GenericObjectWriter<data::SeriesSet>,
        io::writer::factory::make<SeriesSet>
    );

    /// Constructor
    IO_DICOM_API SeriesSet();

    /// Destructor
    IO_DICOM_API ~SeriesSet() override = default;

    /**
     * @brief Manage writing tools to save every series.
     */
    IO_DICOM_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    IO_DICOM_API std::string extension() const override;

    /// Get Fiducials Export Mode
    const io::dicom::writer::Series::FiducialsExportMode& getFiducialsExportMode() const
    {
        return m_fiducialsExportMode;
    }

    /// Set Fiducials Export Mode
    void setFiducialsExportMode(const io::dicom::writer::Series::FiducialsExportMode& fiducialsExportMode)
    {
        m_fiducialsExportMode = fiducialsExportMode;
    }

protected:

    /// Fiducials Export Mode
    io::dicom::writer::Series::FiducialsExportMode m_fiducialsExportMode;
};

} // namespace writer

} // namespace sight::io::dicom
