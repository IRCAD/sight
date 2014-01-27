/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_GDCM_DICOMSERIESDBREADER_HPP__
#define __FWDICOMIOEXT_GDCM_DICOMSERIESDBREADER_HPP__

#include <gdcmScanner.h>

#include <fwCore/macros.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwData
{
class Array;
}

namespace fwMedData
{
class SeriesDB;
class Equipment;
class Patient;
class Series;
class SeriesDB;
class Study;
}

namespace fwDicomData
{
class DicomSeries;
}

namespace fwDicomIOExt
{
namespace gdcm
{

/**
 * @class DicomSeriesDBReader
 * @brief Reads DICOM data from a directory path in order to create a SeriesDB object containing DicomSeries.
 * @date  2013.
 */
class DicomSeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser
{

public :

    typedef std::vector< std::string > SeriesFilesType;

    typedef std::map< std::string, SeriesFilesType > MapSeriesType;

    fwCoreClassDefinitionsWithFactoryMacro(
                    (DicomSeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >),
                    (()),
                    ::fwDataIO::reader::factory::New< DicomSeriesDBReader >
                    );

    fwCoreAllowSharedFromThis();

    /// Does nothing
    FWDICOMIOEXT_API DicomSeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Does nothing
    FWDICOMIOEXT_API ~DicomSeriesDBReader();

    /// Reads DICOM data from configured path and fills SeriesDB object. Use lazy reading process to read images.
    FWDICOMIOEXT_API void read();

private :

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     *
     * @param seriesDB SeriesDB object to be filled with DICOM data
     * @param filenames files to extract DICOM data from
     */
    void addSeries( const SPTR( ::fwMedData::SeriesDB ) &seriesDB, const std::vector< std::string > &filenames);

    /// Searches and sets equipment information
    void fillEquipment( ::gdcm::Scanner & scanner, const std::string & dcmFile, SPTR( ::fwMedData::Equipment ) equipment);

    /// Searches and sets study information
    void fillStudy( ::gdcm::Scanner & scanner, const std::string & dcmFile, SPTR( ::fwMedData::Study ) study );

    /// Searches and sets patient information
    void fillPatient( ::gdcm::Scanner & scanner, const std::string & dcmFile, SPTR( ::fwMedData::Patient ) patient );

    /// Searches and sets series information
    void fillSeries( ::gdcm::Scanner & scanner, const std::string & dcmFile, SPTR( ::fwMedData::Series ) series );

    /// Searches and sets Dicom series information
    void fillDicomSeries( const SeriesFilesType & seriesFiles, SPTR( ::fwDicomData::DicomSeries ) series );

    /// Select some dicom tags and scan information in all filenames
    void scanFiles( ::gdcm::Scanner & scanner, const std::vector< std::string > & filenames );

    /**
     * @brief Convert a file to binary array
     * @param filename file to convert
     */
    SPTR(::fwData::Array) convertToBinary(const std::string& filename);
};

} // namespace gdcm
} // namespace fwDicomIOExt

#endif // __FWDICOMIOEXT_GDCM_DICOMSERIESDBREADER_HPP__
