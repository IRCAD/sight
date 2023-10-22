/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/config.hpp"

#include <core/log/logger.hpp>
#include <core/macros.hpp>

#include <gdcmMediaStorage.h>

#include <memory>

namespace sight::data
{

class dicom_series;
class series;

} // namespace sight::data

namespace sight::io::dicom::container
{

/**
 * @brief This class defines a DICOM SOP instance. It is useful during the whole writing process.
 * This class allows to share data between module writers.
 */
class IO_DICOM_CLASS_API DicomInstance
{
public:

    typedef  std::shared_ptr<DicomInstance> sptr;

    /// SOP Instance Container Type
    typedef std::vector<std::string> SOPInstanceUIDContainerType;

    /// Constructor
    IO_DICOM_API DicomInstance();

    /**
     * @brief Constructor
     * @param[in] series Series from which the instance is created
     * @param[in] isMultiFiles Set whether the instance must be split in several files or not
     * @param[in] logger Logger
     */
    IO_DICOM_API DicomInstance(
        const CSPTR(data::series)& _series,
        SPTR(core::log::logger)_logger = nullptr,
        bool _is_multi_files           = true
    );

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     * @param[in] logger Logger
     */
    IO_DICOM_API DicomInstance(
        const CSPTR(data::dicom_series)& _dicom_series,
        SPTR(core::log::logger)_logger = nullptr
    );

    /// Copy constructor
    IO_DICOM_API DicomInstance(const DicomInstance& _dicom_instance);

    /// Destructor
    IO_DICOM_API virtual ~DicomInstance();

    /// Get the flag on multi-files state of an image series
    [[nodiscard]] bool getIsMultiFiles() const
    {
        return m_isMultiFiles;
    }

    /// Set the flag on multi-files state of an image series
    void setIsMultiFiles(bool _is_multi_files)
    {
        m_isMultiFiles = _is_multi_files;
    }

    /// Get SOP Class UID
    [[nodiscard]] const std::string& getSOPClassUID() const
    {
        return m_sop_classUID;
    }

    /// Set SOP Class UID
    void setSOPClassUID(const std::string& _sop_class_uid)
    {
        m_sop_classUID = _sop_class_uid;
    }

    /// Get Series Instance UID
    [[nodiscard]] const std::string& getSeriesInstanceUID() const
    {
        return m_seriesInstanceUID;
    }

    /// Set Series Instance UID
    void setSeriesInstanceUID(const std::string& _series_instance_uid)
    {
        m_seriesInstanceUID = _series_instance_uid;
    }

    /// Get Study Instance UID
    [[nodiscard]] const std::string& getStudyInstanceUID() const
    {
        return m_studyInstanceUID;
    }

    /// Set Study Instance UID
    void setStudyInstanceUID(const std::string& _study_instance_uid)
    {
        m_studyInstanceUID = _study_instance_uid;
    }

    /// Get Study Instance UID
    [[nodiscard]] const std::string& getFrameOfReferenceUID() const
    {
        return m_frameOfReferenceUID;
    }

    /// Set Study Instance UID
    void setFrameOfReferenceUID(const std::string& _frame_of_reference_uid)
    {
        m_frameOfReferenceUID = _frame_of_reference_uid;
    }

    /// Get SOP Instance UID container
    [[nodiscard]] const SOPInstanceUIDContainerType& getSOPInstanceUIDContainer() const
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Get SOP Instance UID container
    SOPInstanceUIDContainerType& getSOPInstanceUIDContainer()
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Set SOP Instance UID Container
    void setSOPInstanceUIDContainer(const SOPInstanceUIDContainerType& _sop_instance_uid_container)
    {
        m_SOPInstanceUIDContainer = _sop_instance_uid_container;
    }

protected:

    /**
     * @brief Compute sop_classUID
     * @param[in] series Series
     */
    void computesop_classUID(const CSPTR(data::series)& _series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] series Series
     */
    void generateSOPInstanceUIDs(const CSPTR(data::series)& _series);

    /**
     * @brief Extract 'SOP Instance UIDs' and 'Frame of Reference UID' from a DICOM series
     * @param[in] dicomSeries DICOM Series from which the UIDs are extracted
     */
    void readUIDFromDicomSeries(const CSPTR(data::dicom_series)& _dicom_series);

private:

    /// Define if the instance must be split in several files
    bool m_isMultiFiles {true};

    /// SOP Class UID
    std::string m_sop_classUID;

    /// Study Instance UID
    std::string m_studyInstanceUID;

    /// Series Instance UID
    std::string m_seriesInstanceUID;

    /// Frame Of Reference UID
    std::string m_frameOfReferenceUID;

    /// SOP Instance UID container
    SOPInstanceUIDContainerType m_SOPInstanceUIDContainer;

    /// Logger
    SPTR(core::log::logger) m_logger;
};

} // namespace sight::io::dicom::container
