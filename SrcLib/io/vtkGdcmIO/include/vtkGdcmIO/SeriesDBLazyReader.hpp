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

/// Reads a DICOM data from a directory path in order to create a SeriesDB object in a lazy mode.
class SeriesDBLazyReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                           public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                           public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >
{

public:

    typedef std::vector< std::string > SeriesFilesType;

    typedef std::map< std::string, SeriesFilesType > MapSeriesType;

    fwCoreClassMacro(SeriesDBLazyReader, ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                     ::fwDataIO::reader::factory::New< SeriesDBLazyReader >)

    fwCoreAllowSharedFromThis()

    /// Creates the reader.
    VTKGDCMIO_API SeriesDBLazyReader(::fwDataIO::reader::IObjectReader::Key);

    /// Destroyes the reader.
    VTKGDCMIO_API ~SeriesDBLazyReader();

    /// Reads DICOM data from configured path and fills SeriesDB object. Use lazy reading process to read images.
    VTKGDCMIO_API void read() override;

    /// @return internal job.
    VTKGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    /**
     * @brief Creates a SeriesDB from the data contained in the given directory path.
     * @param _dicomDir path from which data is read
     * @return a new SeriesDB object
     */
    SPTR( ::fwMedData::SeriesDB ) createSeriesDB( const std::filesystem::path& _dicomDir );

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     * @param _seriesDB SeriesDB object to be filled with DICOM data
     * @param _filenames files to extract DICOM data from
     */
    void addSeries( const SPTR(::fwMedData::SeriesDB)& _seriesDB, const std::vector< std::string >& _filenames);

    /**
     * @brief Tries estimating Z spacing of an image DICOM, returns 0 if it is not a success.
     * @param _seriesFiles the sorted list of all series fiels.
     * @return The estimated Z spacing.
     */
    double computeZSpacing(const SeriesFilesType& _seriesFiles);

    /**
     * @brief Searches and sets image information: window/center, correct spacing after preprocessImage(not read image
     * buffer).
     * @param _scanner the gdcm scanner.
     * @param _seriesFiles the sorted list of all series files.
     * @param _dcmFile the main file.
     * @param _img the image where fill data.
     */
    void fillImage(gdcm::Scanner& scanner, const SeriesFilesType& _seriesFiles, const std::string& _dcmFile,
                   ::fwData::Image::sptr img);

    /**
     * @brief Searches and sets another image information (origin, spacing, type, size, nbComponent).
     * @param _img the image where fill data.
     * @param _files the sorted list of all series files.
     */
    void preprocessImage(const ::fwData::Image::sptr& _img, const SeriesFilesType& _files);

    /**
     * @brief Searches and sets equipment information.
     * @param _scanner the gdcm scanner.
     * @param _dcmFile the main file.
     * @param _equipment the equipment where fill data.
     */
    void fillEquipment(gdcm::Scanner& _scanner, const std::string& _dcmFile, SPTR(::fwMedData::Equipment) _equipment);

    /**
     * @brief Searches and sets study information.
     * @param _scanner the gdcm scanner.
     * @param _dcmFile the main file.
     * @param _study the study where fill data.
     */
    void fillStudy(gdcm::Scanner& _scanner, const std::string& _dcmFile, SPTR(::fwMedData::Study) _study);

    /**
     * @brief Searches and sets patient information.
     * @param _scanner the gdcm scanner.
     * @param _dcmFile the main file.
     * @param _patient the patient where fill data.
     */
    void fillPatient(gdcm::Scanner& _scanner, const std::string& _dcmFile, SPTR(::fwMedData::Patient) _patient);

    /**
     * @brief Searches and sets series information.
     * @param _scanner the gdcm scanner.
     * @param _dcmFile the main file.
     * @param _imgSeries the image series where fill data.
     */
    void fillSeries(gdcm::Scanner& _scanner, const std::string& _dcmFile, SPTR(::fwMedData::ImageSeries) _imgSeries);

    /**
     * @brief Selects some DICOM tags and scan information in all filenames.
     * @param _scanner the gdcm scanner.
     * @param _filenames files where scan DICOM tags.
     */
    void scanFiles(gdcm::Scanner& _scanner, const std::vector< std::string >& _filenames);

    /// Contains the internal job.
    SPTR(::fwJobs::Observer) m_job;
};

} // namespace vtkGdcmIO
