/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__


#include <fwTools/ProgressAdviser.hpp>
#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include "fwDicomIOExt/dcmtk/reader/IObjectReader.hpp"
#include "fwDicomIOExt/config.hpp"

namespace fwMedData
{
class SeriesDB;
class Patient;
class Study;
class Equipment;
class Series;
}

class DcmDataset;


namespace fwDicomIOExt
{
namespace dcmtk
{

/**
 * @class SeriesDBReader
 *
 * @brief Reads DICOM data from a directory path in order to create a SeriesDB object.
 */
class SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                             public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::IObjectReader > ,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(
                    (SeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >),
                    (()),
                    ::fwDataIO::reader::factory::New< SeriesDBReader >
                    );
    fwCoreAllowSharedFromThis();


    typedef std::map< std::string, SPTR(::fwMedData::Patient) > PatientMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Study) > StudyMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Equipment) > EquipmentMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Series) > SeriesMapType;
    typedef std::map< SPTR(::fwMedData::Series),
            ::fwDicomIOExt::dcmtk::reader::IObjectReader::sptr > ObjectReaderMapType;

    FWDICOMIOEXT_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    FWDICOMIOEXT_API ~SeriesDBReader();

    /**
     * @brief Reads DICOM data from configured path and fills SeriesDB object.
     */
    FWDICOMIOEXT_API void read();

private :

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     *
     * @param seriesDB SeriesDB object to be filled with DICOM data
     * @param filenames files to extract DICOM data from
     */
    void addSeries( const SPTR( ::fwMedData::SeriesDB ) &seriesDB, const std::vector< std::string > &filenames);

    /**
     * @brief Create a patient from the dataset and store it in the patient map
     * @param dataset Dataset from which the patient is created
     */
    SPTR(::fwMedData::Patient) createPatient(DcmDataset* dataset);

    /**
     * @brief Create a study from the dataset and store it in the study map
     * @param dataset Dataset from which the study is created
     */
    SPTR(::fwMedData::Study) createStudy(DcmDataset* dataset);

    /**
     * @brief Create an equipment from the dataset and store it in the equipment map
     * @param dataset Dataset from which the equipment is created
     */
    SPTR(::fwMedData::Equipment) createEquipment(DcmDataset* dataset);

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param dataset Dataset from which the series is created
     * @param filename Instance file
     */
    SPTR(::fwMedData::Series) createSeries(DcmDataset* dataset, const std::string& filename);

    /**
     * @brief Create an object reader according to the instance's SOPClassUID.
     * If there is no reader matching the SOPClassUID, the reader is not created.
     * @param[in] readerType SOPClassUID of the instance
     * @param[in] series Series that will be read
     */
    void createObjectReader(const std::string& readerType, SPTR(::fwMedData::Series) series);

    ///Patient Map
    PatientMapType m_patientMap;

    ///Study Map
    StudyMapType m_studyMap;

    ///Equipment Map
    EquipmentMapType m_equipmentMap;

    ///Series Map
    SeriesMapType m_seriesMap;

    ///Object Reader Map
    ObjectReaderMapType m_objectReaderMap;

};

} //dcmtk
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__*/
