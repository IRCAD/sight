/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "vtkGdcmIO/config.hpp"

#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

namespace fwJobs
{
class Observer;
}

namespace fwMedData
{
class ImageSeries;
}

namespace vtkGdcmIO
{

enum class CompressionTypes
{
    RAW = 0, //default
    JPEG,
    JPEG2000,
    JPEGLS,
    RLE
};

/// Writes an ImageSeries in DICOM format.
class ImageSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries >,
                          public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(ImageSeriesWriter, ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries>,
                     ::fwDataIO::writer::factory::New< ImageSeriesWriter >)

    fwCoreAllowSharedFromThis()

    /// Creates the writer.
    VTKGDCMIO_API ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key);

    /// Writes the image series in DICOM format.
    VTKGDCMIO_API void write() override;

    /// @return An empty string.
    VTKGDCMIO_API std::string extension() override;

    /// @return The internal job.
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

    /**
     * @brief Gets DICOM compression type (default RAW).
     * @return The DICOM compression type.
     */
    inline CompressionTypes getCompressionTypes() const
    {
        return m_compressionTypes;
    }

    /**
     * @brief Sets DICOM compression type.
     * @param _type the DICOM compression type.
     */
    inline void setCompressionTypes(CompressionTypes _type)
    {
        m_compressionTypes = _type;
    }

private:

    /// Contains the internal job.
    SPTR(::fwJobs::Observer) m_job;

    /// Defines the DICOM compression type.
    CompressionTypes m_compressionTypes;
};

} // namespace vtkGdcmIO
