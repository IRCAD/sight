/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/log/Logger.hpp>
#include <core/macros.hpp>

#include <gdcmMediaStorage.h>

#include <memory>

namespace sight::data
{

class DicomSeries;

}

namespace sight::data
{

class Series;

}

namespace sight::io::dicom
{

namespace container
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
        const CSPTR(data::Series)& series,
        const SPTR(core::log::Logger)& logger = nullptr,
        bool isMultiFiles                     = true
    );

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     * @param[in] logger Logger
     */
    IO_DICOM_API DicomInstance(
        const CSPTR(data::DicomSeries)& dicomSeries,
        const SPTR(core::log::Logger)& logger = nullptr
    );

    /// Copy constructor
    IO_DICOM_API DicomInstance(const DicomInstance& dicomInstance);

    /// Destructor
    IO_DICOM_API virtual ~DicomInstance();

    /// Get the flag on multi-files state of an image series
    bool getIsMultiFiles() const
    {
        return m_isMultiFiles;
    }

    /// Set the flag on multi-files state of an image series
    void setIsMultiFiles(bool isMultiFiles)
    {
        m_isMultiFiles = isMultiFiles;
    }

    /// Get SOP Class UID
    const std::string& getSOPClassUID() const
    {
        return m_SOPClassUID;
    }

    /// Set SOP Class UID
    void setSOPClassUID(const std::string& SOPClassUID)
    {
        m_SOPClassUID = SOPClassUID;
    }

    /// Get Series Instance UID
    const std::string& getSeriesInstanceUID() const
    {
        return m_seriesInstanceUID;
    }

    /// Set Series Instance UID
    void setSeriesInstanceUID(const std::string& seriesInstanceUID)
    {
        m_seriesInstanceUID = seriesInstanceUID;
    }

    /// Get Study Instance UID
    const std::string& getStudyInstanceUID() const
    {
        return m_studyInstanceUID;
    }

    /// Set Study Instance UID
    void setStudyInstanceUID(const std::string& studyInstanceUID)
    {
        m_studyInstanceUID = studyInstanceUID;
    }

    /// Get Study Instance UID
    const std::string& getFrameOfReferenceUID() const
    {
        return m_frameOfReferenceUID;
    }

    /// Set Study Instance UID
    void setFrameOfReferenceUID(const std::string& frameOfReferenceUID)
    {
        m_frameOfReferenceUID = frameOfReferenceUID;
    }

    /// Get SOP Instance UID container
    const SOPInstanceUIDContainerType& getSOPInstanceUIDContainer() const
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Get SOP Instance UID container
    SOPInstanceUIDContainerType& getSOPInstanceUIDContainer()
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Set SOP Instance UID Container
    void setSOPInstanceUIDContainer(const SOPInstanceUIDContainerType& SOPInstanceUIDContainer)
    {
        m_SOPInstanceUIDContainer = SOPInstanceUIDContainer;
    }

protected:

    /**
     * @brief Compute SOPClassUID
     * @param[in] series Series
     */
    void computeSOPClassUID(const CSPTR(data::Series)& series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] series Series
     */
    void generateSOPInstanceUIDs(const CSPTR(data::Series)& series);

    /**
     * @brief Extract 'SOP Instance UIDs' and 'Frame of Reference UID' from a DICOM series
     * @param[in] dicomSeries DICOM Series from which the UIDs are extracted
     */
    void readUIDFromDicomSeries(const CSPTR(data::DicomSeries)& dicomSeries);

private:

    /// Define if the instance must be split in several files
    bool m_isMultiFiles;

    /// SOP Class UID
    std::string m_SOPClassUID;

    /// Study Instance UID
    std::string m_studyInstanceUID;

    /// Series Instance UID
    std::string m_seriesInstanceUID;

    /// Frame Of Reference UID
    std::string m_frameOfReferenceUID;

    /// SOP Instance UID container
    SOPInstanceUIDContainerType m_SOPInstanceUIDContainer;

    /// Logger
    SPTR(core::log::Logger) m_logger;
};

} //namespace container

} //namespace sight::io::dicom
