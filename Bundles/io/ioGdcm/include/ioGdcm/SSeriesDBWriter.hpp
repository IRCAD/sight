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

#include "ioGdcm/config.hpp"

#include <fwGdcmIO/writer/Series.hpp>

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief DICOM Writer (ImageSeries/ModelSeries)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SSeriesDBWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [::fwData::Vector]: vector containing Series (ImageSeries or ModelSeries) to save in Dicom.
 */
class IOGDCM_CLASS_API SSeriesDBWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceMacro(SSeriesDBWriter,  ::fwIO::IWriter);

    /**
     * @brief Constructor
     */
    IOGDCM_API SSeriesDBWriter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSeriesDBWriter() noexcept override;

    /// Propose a directory selection where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM() override;

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() override;

    /// Does nothing
    IOGDCM_API virtual void stopping() override;

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Write the ImageSeries in DICOM format.
    IOGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a SeriesDB to a folder in DICOM format
     * @param[in] folder DICOM folder
     * @param[in] seriesDB SeriesDB that must be writen
     */
    void saveSeriesDB( const ::boost::filesystem::path folder, SPTR(::fwMedData::SeriesDB) seriesDB );

    /// Select Fiducial Export Mode using a dialog
    bool selectFiducialsExportMode();

    /// Fiducial Export Mode
    ::fwGdcmIO::writer::Series::FiducialsExportMode m_fiducialsExportMode;

};

} // namespace ioGdcm
