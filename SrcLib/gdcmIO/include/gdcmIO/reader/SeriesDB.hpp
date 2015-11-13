/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_SERIESDB_HPP__
#define __GDCMIO_READER_SERIESDB_HPP__

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/config.hpp"

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwServices/IService.hpp>

#include <gdcmScanner.h>

namespace gdcmIO
{

namespace reader
{

/**
 * @class   SeriesDB
 * @brief   This class adds patient(s) from DICOM file(s) to fwData::SeriesDB.
 */
class GDCMIO_CLASS_API SeriesDB : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                                  public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                                  public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader >,
                                  public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SeriesDB)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB > ),
        (()), ::fwDataIO::reader::factory::New< SeriesDB >);

    typedef std::map< std::string, SPTR(::fwMedData::Patient) > PatientMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Study) > StudyMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Equipment) > EquipmentMapType;
    typedef std::vector< SPTR(::fwDicomData::DicomSeries) > DicomSeriesContainerType;
    typedef std::vector< std::string > FilenameContainerType;

    /// Constructor
    GDCMIO_API SeriesDB(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor
    GDCMIO_API ~SeriesDB();

    /// Reads DICOM data from configured path and fills SeriesDB object
    GDCMIO_API void read();

    /**
     * @brief Reads DICOM data from DicomSeries and fills SeriesDB object
     * @param[in] dicomSeriesDB SeriesDB containing DicomSeries that must be read
     * @param[in] notifier Service used to notify changes in SeriesDB
     */
    GDCMIO_API void readFromDicomSeriesDB(::fwMedData::SeriesDB::sptr dicomSeriesDB);

    /**
     * @brief Reads DICOM data from configured path and fills SeriesDB object with DicomSeries
     */
    GDCMIO_API void readDicomSeries();

    /// Return true if a dicomdir file can be read.
    GDCMIO_API bool isDicomDirAvailable();

    /// Return DicomSeries container
    GDCMIO_API DicomSeriesContainerType &getDicomSeries();

    /**
     * @brief Dicom filter type that must be applied prior to the reading process
     * @{ */
    const std::string& getDicomFilterType() const;
    void setDicomFilterType(const std::string& dicomFilterType);
    /**  @} */

    /**
     * @brief Set whether the reader must use the dicomdir file or not
     * @{ */
    bool getDicomdirActivated() const;
    void setDicomdirActivated(bool isDicomdirActivated);
    /**  @} */

private:

    /// Return filenames that must be read
    FilenameContainerType getFilenames();

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     * @param[in] filenames files to extract DICOM data from
     */
    void addSeries(const std::vector< std::string > &filenames);

    /**
     * @brief Create a patient from the dataset and store it in the patient map
     * @param[in] scanner GDCM Scanner used to read information
     * @param[in] filename Filename from which the information must be read
     */
    SPTR(::fwMedData::Patient) createPatient(const ::gdcm::Scanner& scanner, const std::string& filename);

    /**
     * @brief Create a study from the dataset and store it in the study map
     * @param[in] scanner GDCM Scanner used to read information
     * @param[in] filename Filename from which the information must be read
     */
    SPTR(::fwMedData::Study) createStudy(const ::gdcm::Scanner& scanner, const std::string& filename);

    /**
     * @brief Create an equipment from the dataset and store it in the equipment map
     * @param[in] scanner GDCM Scanner used to read information
     * @param[in] filename Filename from which the information must be read
     */
    SPTR(::fwMedData::Equipment) createEquipment(const ::gdcm::Scanner& scanner, const std::string& filename);

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param[in] scanner GDCM Scanner used to read information
     * @param[in] filename Filename from which the information must be read
     */
    void createSeries(const ::gdcm::Scanner& scanner, const std::string& filename);

    /**
     * @brief Convert DicomSeries to Image or Model Series
     * @param[in] notify if true, notify the seriesDB
     */
    void convertDicomSeries(bool notify = false);

    /**
     * @brief Function used to sort DicomSeries
     * @param[in] a First DicomSeries
     * @param[in] b Second DicomSeries
     */
    static bool dicomSeriesComparator(SPTR(::fwDicomData::DicomSeries) a, SPTR(::fwDicomData::DicomSeries) b);

    /**
     * @brief Read value from a scanner
     * @param[in] scanner Scanner used to read the value
     * @param[in] filename Filename from which the information must be read
     * @param[in] tag Tag that must be read
     */
    static std::string getStringValue(const ::gdcm::Scanner& scanner, const std::string& filename,
                                      const gdcm::Tag& tag);

    ///Patient Map
    PatientMapType m_patientMap;

    ///Study Map
    StudyMapType m_studyMap;

    ///Equipment Map
    EquipmentMapType m_equipmentMap;

    ///Object Reader Map
    DicomSeriesContainerType m_dicomSeriesContainer;

    ///True if the reader can use the dicomdir file.
    bool m_isDicomdirActivated;

    ///Dicom filter type that must be applied prior to the reading process
    std::string m_dicomFilterType;

};

//-----------------------------------------------------------------------------

inline const std::string &SeriesDB::getDicomFilterType() const
{
    return m_dicomFilterType;
}

//-----------------------------------------------------------------------------

inline void SeriesDB::setDicomFilterType(const std::string &dicomFilterType)
{
    m_dicomFilterType = dicomFilterType;
}

//-----------------------------------------------------------------------------

inline bool SeriesDB::getDicomdirActivated() const
{
    return m_isDicomdirActivated;
}

//-----------------------------------------------------------------------------

inline void SeriesDB::setDicomdirActivated(bool isDicomdirActivated)
{
    m_isDicomdirActivated = isDicomdirActivated;
}

//-----------------------------------------------------------------------------

}  // namespace reader

}  // namespace gdcmIO

#endif /*__GDCMIO_READER_SERIESDB_HPP__*/
