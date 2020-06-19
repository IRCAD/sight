/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <fwData/Object.hpp>

#include <fwGdcmIO/writer/Series.hpp>

#include <fwIO/IWriter.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>

#include <filesystem>

namespace fwMedData
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
 * - \b data [::fwMedData::ModelSeries]: ModelSeries to save in Dicom.
 */
class IOGDCM_CLASS_API SSurfaceSegmentationWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceMacro(SSurfaceSegmentationWriter,  ::fwIO::IWriter);

    /**
     * @brief Constructor
     */
    IOGDCM_API SSurfaceSegmentationWriter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSurfaceSegmentationWriter() noexcept override;

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
     * @brief Write a Series to a folder in DICOM format
     * @param[in] filename DICOM file name
     * @param[in] model ModelSeries that must be written
     */
    void saveSurfaceSegmentation( const std::filesystem::path filename,
                                  const ::fwMedData::ModelSeries::csptr& model);
};

} // namespace ioGdcm
