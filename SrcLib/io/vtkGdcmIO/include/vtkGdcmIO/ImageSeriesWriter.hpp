/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKGDCMIO_IMAGESERIESWRITER_HPP__
#define __VTKGDCMIO_IMAGESERIESWRITER_HPP__

#include "vtkGdcmIO/config.hpp"

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>

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
 * @class ImageSeriesWriter
 */
class ImageSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ImageSeries >,
                          public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ImageSeriesWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::
                                                                                                         ImageSeries>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ImageSeriesWriter >
                                           );
    fwCoreAllowSharedFromThis();

    /// Constructor. Does nothing
    VTKGDCMIO_API ImageSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Write the image series in DICOM format.
    VTKGDCMIO_API void write();

    /// Return an empty string
    VTKGDCMIO_API std::string  extension();

    /// @return internal job
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const;

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

#endif // __VTKGDCMIO_IMAGESERIESWRITER_HPP__
