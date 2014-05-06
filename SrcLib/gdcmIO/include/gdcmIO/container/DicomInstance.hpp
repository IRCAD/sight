/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMINSTANCE_HPP__
#define __GDCMIO_CONTAINER_DICOMINSTANCE_HPP__

#include <string>
#include <vector>

#include <gdcmMediaStorage.h>

#include <fwData/macros.hpp>

#include "gdcmIO/config.hpp"

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
    GDCMIO_API DicomInstance(SPTR(::fwMedData::Series) series, bool isMultiFiles);

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     */
    GDCMIO_API DicomInstance(SPTR(::fwDicomData::DicomSeries) dicomSeries);

    /// Copy constructor
    GDCMIO_API DicomInstance(const DicomInstance & dicomInstance);

    /// Destructor
    GDCMIO_API virtual ~DicomInstance();

    GDCMIO_API fwGettersSettersDocMacro(IsMultiFiles, isMultiFiles, bool, Flag on multi-files state of an image series);

    GDCMIO_API fwGettersSettersDocMacro(SOPClassUID, SOPClassUID, std::string, SOP Class UID);

    GDCMIO_API fwGettersSettersDocMacro(SeriesInstanceUID, seriesInstanceUID, std::string, Series Instance UID);

    GDCMIO_API fwGettersSettersDocMacro(StudyInstanceUID, studyInstanceUID, std::string, Study Instance UID);

    GDCMIO_API fwGettersSettersDocMacro(SOPInstanceUIDContainer, SOPInstanceUIDContainer, SOPInstanceUIDContainerType,
            SOP Instance UID container);

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
