/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/DicomSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Object.hpp>
#include <data/Series.hpp>

#include <fwGdcmIO/writer/Series.hpp>

#include <io/base/services/IWriter.hpp>

#include <filesystem>

namespace sight::data
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief DICOM 3D Writer (ModelSeries)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SSurfaceSegmentationWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [data::ModelSeries]: ModelSeries to save in Dicom.
 */
class IOGDCM_CLASS_API SSurfaceSegmentationWriter : public sight::io::base::services::IWriter
{

public:
    fwCoreServiceMacro(SSurfaceSegmentationWriter, sight::io::base::services::IWriter)

    /**
     * @brief Constructor
     */
    IOGDCM_API SSurfaceSegmentationWriter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSurfaceSegmentationWriter() noexcept override;

    /**
     * @brief Propose a directory selection where to save the DICOM files.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOGDCM_API virtual void configureWithIHM() override;

    /// Propose a directory selection where to save the DICOM files.
    IOGDCM_API virtual void openLocationDialog() override;

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
    IOGDCM_API io::base::services::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a Series to a folder in DICOM format
     * @param[in] filename DICOM file name
     * @param[in] model ModelSeries that must be written
     */
    void saveSurfaceSegmentation( const std::filesystem::path filename,
                                  const data::ModelSeries::csptr& model);
};

} // namespace ioGdcm
