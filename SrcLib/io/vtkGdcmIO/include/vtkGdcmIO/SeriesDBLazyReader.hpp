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

#include <fwCore/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <gdcmScanner.h>

namespace fwMedData
{
class SeriesDB;
class Equipment;
class ImageSeries;
class Patient;
class Series;
class SeriesDB;
class Study;
}

namespace fwJobs
{
class Observer;
}

namespace vtkGdcmIO
{

/**
 *
 * @brief Reads DICOM data from a directory path in order to create a SeriesDB object in lazy mode.
 */
class SeriesDBLazyReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                           public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                           public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >
{

public:

    typedef std::vector< std::string > SeriesFilesType;

    typedef std::map< std::string, SeriesFilesType > MapSeriesType;

    fwCoreClassMacro(SeriesDBLazyReader, ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                     ::fwDataIO::reader::factory::New< SeriesDBLazyReader >);

    fwCoreAllowSharedFromThis();

    /// Does nothing
    VTKGDCMIO_API SeriesDBLazyReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Does nothing
    VTKGDCMIO_API ~SeriesDBLazyReader();

    /// Reads DICOM data from configured path and fills SeriesDB object. Use lazy reading process to read images.
    VTKGDCMIO_API void read() override;

    /// @return internal job
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    /**
     * @brief Creates a SeriesDB from the data contained in the given directory path.
     *
     * @param directory path from which data is read
     *
     * @return a new SeriesDB object
     */
    SPTR( ::fwMedData::SeriesDB ) createSeriesDB( const ::boost::filesystem::path& dicomDir );

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     *
     * @param seriesDB SeriesDB object to be filled with DICOM data
     * @param filenames files to extract DICOM data from
     */
    void addSeries( const SPTR( ::fwMedData::SeriesDB )& seriesDB, const std::vector< std::string >& filenames);

    /// Tries estimating Z spacing of an image dicom, returns 0 if it is not a success
    double computeZSpacing( const SeriesFilesType& seriesFiles );

    /// Searches and sets image information: window/center, correct spacing after preprocessImage(not read image buffer)
    void fillImage( gdcm::Scanner& scanner, const SeriesFilesType& seriesFiles,
                    const std::string& dcmFile, ::fwData::Image::sptr img );

    /// Searches and sets another image information ( origin, spacing, type, size, nbComponent )
    void preprocessImage( const ::fwData::Image::sptr& img, const SeriesFilesType& files );

    /// Searches and sets equipment information
    void fillEquipment( gdcm::Scanner& scanner, const std::string& dcmFile, SPTR( ::fwMedData::Equipment ) equipment);

    /// Searches and sets study information
    void fillStudy( gdcm::Scanner& scanner, const std::string& dcmFile, SPTR( ::fwMedData::Study ) study );

    /// Searches and sets patient information
    void fillPatient( gdcm::Scanner& scanner, const std::string& dcmFile, SPTR( ::fwMedData::Patient ) patient );

    /// Searches and sets series information
    void fillSeries( gdcm::Scanner& scanner, const std::string& dcmFile, SPTR( ::fwMedData::Series ) series );

    /// Select some dicom tags and scan information in all filenames
    void scanFiles( gdcm::Scanner& scanner, const std::vector< std::string >& filenames );

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace vtkGdcmIO
