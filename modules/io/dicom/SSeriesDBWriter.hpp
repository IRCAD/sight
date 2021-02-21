/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/io/dicom/config.hpp"

#include <io/base/service/IWriter.hpp>
#include <io/dicom/writer/Series.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{
class SeriesDB;
}

namespace sight::module::io::dicom
{

/**
 * @brief DICOM Writer (ImageSeries/ModelSeries)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::sight::module::io::dicom::SSeriesDBWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::Vector]: vector containing Series (ImageSeries or ModelSeries) to save in Dicom.
 */
class MODULE_IO_DICOM_CLASS_API SSeriesDBWriter : public sight::io::base::service::IWriter
{

public:
    fwCoreServiceMacro(SSeriesDBWriter, sight::io::base::service::IWriter)

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOM_API SSeriesDBWriter() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOM_API virtual ~SSeriesDBWriter() noexcept override;

    /**
     * @brief Propose a directory selection where to save the DICOM files.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_DICOM_API virtual void configureWithIHM() override;

    /// Propose a directory selection where to save the DICOM files.
    MODULE_IO_DICOM_API virtual void openLocationDialog() override;

protected:

    /// Does nothing
    MODULE_IO_DICOM_API virtual void starting() override;

    /// Does nothing
    MODULE_IO_DICOM_API virtual void stopping() override;

    /// Configuring method. This method is used to configure the service.
    MODULE_IO_DICOM_API virtual void configuring() override;

    /// Write the ImageSeries in DICOM format.
    MODULE_IO_DICOM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    MODULE_IO_DICOM_API sight::io::base::service::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a SeriesDB to a folder in DICOM format
     * @param[in] folder DICOM folder
     * @param[in] seriesDB SeriesDB that must be writen
     */
    void saveSeriesDB( const std::filesystem::path folder, SPTR(data::SeriesDB) seriesDB );

    /// Select Fiducial Export Mode using a dialog
    bool selectFiducialsExportMode();

    /// Fiducial Export Mode
    sight::io::dicom::writer::Series::FiducialsExportMode m_fiducialsExportMode;

};

} // namespace sight::module::io::dicom
