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
#include "io/dicom/writer/Series.hpp"

#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace sight::io::dicom
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 */
class SeriesDB : public io::base::writer::GenericObjectWriter<data::SeriesDB>,
                 public core::location::SingleFolder,
                 public core::tools::ProgressAdviser
{
public:

    SIGHT_DECLARE_CLASS(
        SeriesDB,
        io::base::writer::GenericObjectWriter<data::SeriesDB>,
        io::base::writer::factory::New<SeriesDB>
    );

    /// Constructor
    IO_DICOM_API SeriesDB(io::base::writer::IObjectWriter::Key key);

    /// Destructor
    IO_DICOM_API ~SeriesDB();

    /**
     * @brief Manage writing tools to save every series.
     */
    IO_DICOM_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    IO_DICOM_API std::string extension() override;

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

    /**
     * @brief Function used to sort Series
     * @param[in] a First Series
     * @param[in] b Second Series
     */
    static bool seriesComparator(
        const data::Series::csptr& a,
        const data::Series::csptr& b
    );

    /// Fiducials Export Mode
    io::dicom::writer::Series::FiducialsExportMode m_fiducialsExportMode;
};

} // namespace writer

} // namespace sight::io::dicom
