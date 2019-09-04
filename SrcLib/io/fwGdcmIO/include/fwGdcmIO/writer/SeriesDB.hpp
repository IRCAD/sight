/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/writer/Series.hpp"

#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwGdcmIO
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 */
class SeriesDB : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB >,
                 public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                 public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SeriesDB)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB >),
        ::fwDataIO::writer::factory::New< SeriesDB >);

    /// Constructor
    FWGDCMIO_API SeriesDB(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor
    FWGDCMIO_API ~SeriesDB();

    /**
     * @brief Manage writing tools to save every series.
     */
    FWGDCMIO_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    FWGDCMIO_API std::string extension() override;

    /// Get Fiducials Export Mode
    const ::fwGdcmIO::writer::Series::FiducialsExportMode& getFiducialsExportMode() const
    {
        return m_fiducialsExportMode;
    }

    /// Set Fiducials Export Mode
    void setFiducialsExportMode(const ::fwGdcmIO::writer::Series::FiducialsExportMode& fiducialsExportMode)
    {
        m_fiducialsExportMode = fiducialsExportMode;
    }

protected:
    /**
     * @brief Function used to sort Series
     * @param[in] a First Series
     * @param[in] b Second Series
     */
    static bool seriesComparator(const ::fwMedData::Series::csptr& a,
                                 const ::fwMedData::Series::csptr& b);

    /// Fiducials Export Mode
    ::fwGdcmIO::writer::Series::FiducialsExportMode m_fiducialsExportMode;
};

} // namespace writer

} // namespace fwGdcmIO
