/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMINSTANCE_HPP__
#define __GDCMIO_CONTAINER_DICOMINSTANCE_HPP__

#include "gdcmIO/config.hpp"

#include <gdcmMediaStorage.h>

#include <string>
#include <vector>


namespace fwDicomData
{
class DicomSeries;
}

namespace fwMedData
{
class Series;
}

namespace gdcmIO
{
namespace container
{

/**
 * @class DicomInstance
 * @brief This class defines a DICOM SOP instance. It is useful during the whole writing process.
 * This class allows to share data between module writers.
 */
class GDCMIO_CLASS_API DicomInstance
{
public:

    typedef std::vector< std::string > SOPInstanceUIDContainerType;

    /// Constructor
    GDCMIO_API DicomInstance();

    /**
     * @brief Constructor
     * @param[in] series Series from which the instance is created
     * @param[in] isMultiFiles Set whether the instance must be split in several files or not
     */
    GDCMIO_API DicomInstance(SPTR(::fwMedData::Series)series, bool isMultiFiles);

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     */
    GDCMIO_API DicomInstance(SPTR(::fwDicomData::DicomSeries)dicomSeries);

    /// Copy constructor
    GDCMIO_API DicomInstance(const DicomInstance & dicomInstance);

    /// Destructor
    GDCMIO_API virtual ~DicomInstance();

    /**
     * @brief Flag on multi-files state of an image series
     * @{ */
    bool getIsMultiFiles() const
    {
        return this->m_isMultiFiles;
    }
    void setIsMultiFiles(bool isMultiFiles)
    {
        m_isMultiFiles = isMultiFiles;
    }
    /**  @} */

    /**
     * @brief Series Instance UID
     * @{ */
    const std::string& getSeriesInstanceUID() const
    {
        return m_seriesInstanceUID;
    }
    void setSeriesInstanceUID(const std::string& seriesInstanceUid)
    {
        m_seriesInstanceUID = seriesInstanceUid;
    }
    /**  @} */

    /**
     * @brief SOP Class UID
     * @{ */
    const std::string& getSOPClassUID() const
    {
        return m_SOPClassUID;
    }
    void setSOPClassUID(const std::string& sopClassUid)
    {
        m_SOPClassUID = sopClassUid;
    }
    /**  @} */

    /**
     * @brief SOP Instance UID container
     * @{ */
    const SOPInstanceUIDContainerType& getSOPInstanceUIDContainer() const
    {
        return m_SOPInstanceUIDContainer;
    }
    SOPInstanceUIDContainerType& getSOPInstanceUIDContainer()
    {
        return m_SOPInstanceUIDContainer;
    }
    void setSOPInstanceUIDContainer(const SOPInstanceUIDContainerType& sopInstanceUidContainer)
    {
        m_SOPInstanceUIDContainer = sopInstanceUidContainer;
    }
    /**  @} */

    /**
     * @brief Study Instance UID
     * @{ */
    const std::string& getStudyInstanceUID() const
    {
        return m_studyInstanceUID;
    }
    void setStudyInstanceUID(const std::string& studyInstanceUid)
    {
        m_studyInstanceUID = studyInstanceUid;
    }
    /**  @} */


protected:
    /**
     * @brief Compute SOPClassUID
     * @param[in] series Series
     */
    void computeSOPClassUID(SPTR(::fwMedData::Series) series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] series Series
     */
    void generateSOPInstanceUIDs(SPTR(::fwMedData::Series) series);

private:

    /// Define if the instance must be split in several files
    bool m_isMultiFiles;

    /// SOP Class UID
    std::string m_SOPClassUID;

    /// Study Instance UID
    std::string m_studyInstanceUID;

    /// Series Instance UID
    std::string m_seriesInstanceUID;

    /// SOP Instance UID container
    SOPInstanceUIDContainerType m_SOPInstanceUIDContainer;

};

}
//namespace container
}//namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMINSTANCE_HPP__ */
