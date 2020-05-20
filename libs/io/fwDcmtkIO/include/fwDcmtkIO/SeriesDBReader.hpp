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

#include "fwDcmtkIO/config.hpp"
#include "fwDcmtkIO/reader/IObjectReader.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwServices/IService.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwMedData
{
class SeriesDB;
class Patient;
class Study;
class Equipment;
class Series;
}

class DcmDataset;

namespace fwDcmtkIO
{

/**
 *
 * @brief Reads DICOM data from a directory path in order to create a SeriesDB object.
 */
class FWDCMTKIO_CLASS_API SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                                           public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                                           public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::
                                                                                        IObjectReader >,
                                           public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassMacro(
        SeriesDBReader,
        ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
        ::fwDataIO::reader::factory::New< SeriesDBReader >
        );
    fwCoreAllowSharedFromThis();

    typedef std::map< std::string, SPTR(::fwMedData::Patient) > PatientMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Study) > StudyMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Equipment) > EquipmentMapType;
    typedef std::vector< SPTR(::fwMedData::DicomSeries) > DicomSeriesContainerType;
    typedef std::vector< std::string > FilenameContainerType;
    typedef std::vector< std::string > SupportedSOPClassContainerType;

    FWDCMTKIO_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    FWDCMTKIO_API ~SeriesDBReader();

    /// Reads DICOM data from configured path and fills SeriesDB object
    FWDCMTKIO_API void read() override;

    /**
     * @brief Reads DICOM data from DicomSeries an fills SeriesDB object
     * @param[in] dicomSeriesDB SeriesDB containing DicomSeries that must be read
     * @param[in] notifier Service used to notify changes in SeriesDB
     */
    FWDCMTKIO_API void readFromDicomSeriesDB(CSPTR(::fwMedData::SeriesDB) dicomSeriesDB,
                                             ::fwServices::IService::sptr notifier = ::fwServices::IService::sptr());

    /// Reads DICOM data from configured path and fills SeriesDB object with DicomSeries
    FWDCMTKIO_API void readDicomSeries();

    /// Return true if a dicomdir file can be read.
    FWDCMTKIO_API bool isDicomDirAvailable();

    /// Return DicomSeries container
    FWDCMTKIO_API DicomSeriesContainerType& getDicomSeries();

    /// Get Set whether the reader must use the dicomdir file or not
    const bool& getDicomdirActivated() const
    {
        return m_isDicomdirActivated;
    }

    /// Set Set whether the reader must use the dicomdir file or not
    void setDicomdirActivated(const bool& isDicomdirActivated)
    {
        m_isDicomdirActivated = isDicomdirActivated;
    }

    /// Get Dicom filter type that must be applied prior the reading process
    const std::string& getDicomFilterType() const
    {
        return m_dicomFilterType;
    }

    /// Set Dicom filter type that must be applied prior the reading process
    void setDicomFilterType(const std::string& dicomFilterType)
    {
        m_dicomFilterType = dicomFilterType;
    }

    /// Get Supported SOP Class
    const SupportedSOPClassContainerType& getSupportedSOPClassContainer() const
    {
        return m_supportedSOPClassContainer;
    }

    /// Set Supported SOP Class
    void setSupportedSOPClassContainer(const SupportedSOPClassContainerType& supportedSOPClassContainer)
    {
        m_supportedSOPClassContainer = supportedSOPClassContainer;
    }

private:

    /// Return filenames that must be read
    FilenameContainerType getFilenames();

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     *
     * @param[in] seriesDB SeriesDB object to be filled with DICOM data
     * @param[in] filenames files to extract DICOM data from
     */
    void addSeries(const std::vector< std::string >& filenames);

    /**
     * @brief Create a patient from the dataset and store it in the patient map
     * @param[in] dataset Dataset from which the patient is created
     */
    SPTR(::fwMedData::Patient) createPatient(DcmDataset* dataset);

    /**
     * @brief Create a study from the dataset and store it in the study map
     * @param[in] dataset Dataset from which the study is created
     */
    SPTR(::fwMedData::Study) createStudy(DcmDataset* dataset);

    /**
     * @brief Create an equipment from the dataset and store it in the equipment map
     * @param[in] dataset Dataset from which the equipment is created
     */
    SPTR(::fwMedData::Equipment) createEquipment(DcmDataset* dataset);

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param[in] dataset Dataset from which the series is created
     * @param[in] filename Instance file
     */
    void createSeries(DcmDataset* dataset, const std::string& filename);

    /**
     * @brief Convert DicomSeries to Image or Model Series
     * @param[in] dicomSeries Dicom Series that must be converted
     * @param[in] notifier Service used to notify the modification
     */
    void convertDicomSeries(CSPTR(::fwMedData::DicomSeries) dicomSeries,
                            ::fwServices::IService::sptr notifier = ::fwServices::IService::sptr());

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

    ///Dicom filter type that must be applied prior the reading process
    std::string m_dicomFilterType;

    ///Supported SOP Class container
    SupportedSOPClassContainerType m_supportedSOPClassContainer;

};

} // namespace fwDcmtkIO
