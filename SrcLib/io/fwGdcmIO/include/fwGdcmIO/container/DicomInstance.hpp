/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__
#define __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>
#include <fwLog/Logger.hpp>

#include <gdcmMediaStorage.h>


namespace fwMedData
{
class DicomSeries;
}

namespace fwMedData
{
class Series;
}

namespace fwGdcmIO
{
namespace container
{

/**
 * @brief This class defines a DICOM SOP instance. It is useful during the whole writing process.
 * This class allows to share data between module writers.
 */
class FWGDCMIO_CLASS_API DicomInstance
{
public:

    /// SOP Instance Container Type
    typedef std::vector< std::string > SOPInstanceUIDContainerType;

    /// Constructor
    FWGDCMIO_API DicomInstance();

    /**
     * @brief Constructor
     * @param[in] series Series from which the instance is created
     * @param[in] isMultiFiles Set whether the instance must be split in several files or not
     * @param[in] logger Logger
     */
    FWGDCMIO_API DicomInstance(const SPTR(::fwMedData::Series)& series,
                               const SPTR(::fwLog::Logger)& logger = nullptr,
                               bool isMultiFiles = true);

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     * @param[in] logger Logger
     */
    FWGDCMIO_API DicomInstance(const SPTR(::fwMedData::DicomSeries)& dicomSeries,
                               const SPTR(::fwLog::Logger)& logger = nullptr);

    /// Copy constructor
    FWGDCMIO_API DicomInstance(const DicomInstance& dicomInstance);

    /// Destructor
    FWGDCMIO_API virtual ~DicomInstance();

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
    SOPInstanceUIDContainerType& getRefSOPInstanceUIDContainer()
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
    void computeSOPClassUID(const CSPTR(::fwMedData::Series)& series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] series Series
     */
    void generateSOPInstanceUIDs(const CSPTR(::fwMedData::Series)& series);

    /**
     * @brief Extract 'SOP Instance UIDs' and 'Frame of Reference UID' from a DICOM series
     * @param[in] dicomSeries DICOM Series from which the UIDs are extracted
     */
    void readUIDFromDicomSeries(const CSPTR(::fwMedData::DicomSeries)& dicomSeries);

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
    SPTR(::fwLog::Logger) m_logger;
};

} //namespace container
} //namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__ */
