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

/**
 * @brief Write an ImageSeries in DICOM format.
 */
class ImageSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries >,
                          public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ImageSeriesWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::
                                                                                                         ImageSeries>),
                                           ::fwDataIO::writer::factory::New< ImageSeriesWriter >
                                           )
    fwCoreAllowSharedFromThis()

    /// Constructor. Does nothing
    VTKGDCMIO_API ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Write the image series in DICOM format.
    VTKGDCMIO_API void write() override;

    /// Return an empty string
    VTKGDCMIO_API std::string  extension() override;

    /// @return internal job
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

    /// Get DICOM compression type (default RAW)
    CompressionTypes getCompressionTypes() const
    {
        return m_compressionTypes;
    }

    /// Set DICOM compression type
    void setCompressionTypes(CompressionTypes type)
    {
        m_compressionTypes = type;
    }
private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
    CompressionTypes m_compressionTypes;
};

} // namespace vtkGdcmIO
