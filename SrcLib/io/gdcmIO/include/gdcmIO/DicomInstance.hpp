/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMINSTANCE_HPP_
#define _GDCMIO_DICOMINSTANCE_HPP_

#include <string>
#include <vector>

#include <gdcmMediaStorage.h>

#include "fwData/macros.hpp"

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

/**
 * @brief   This class defines a DICOM SOP instance.
 *
 * It is useful during all writing process. This class
 * allows to share data between writers.
 *
 * @class   DicomInstance.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomInstance
{
public:

    GDCMIO_API DicomInstance();

    GDCMIO_API virtual ~DicomInstance();

    GDCMIO_API DicomInstance( const DicomInstance & dicomInstance);

    GDCMIO_API fwGettersSettersDocMacro(SOPClassUID, SOPClassUID, std::string, The SOP class UID.);

    GDCMIO_API void setSOPClassUID(const ::gdcm::MediaStorage::MSType a_MSType);

    GDCMIO_API fwGettersSettersDocMacro(SOPInstanceUID, SOPInstanceUID, std::string, The SOP instance UID.);

    GDCMIO_API fwGettersSettersDocMacro(StudyInstanceUID, studyInstanceUID, std::string, The study instance UID.);

    GDCMIO_API fwGettersSettersDocMacro(SeriesInstanceUID, seriesInstanceUID, std::string, The series instance UID.);

    GDCMIO_API fwGettersSettersDocMacro(FrameOfRefUID, frameOfRefUID, std::string, The frame of reference UID.);

    GDCMIO_API fwGettersSettersDocMacro(InstanceNumber, instanceNumber, std::string, The instance number.);

    GDCMIO_API fwGettersSettersDocMacro(Modality, modality, std::string, The modality of image.);

    GDCMIO_API fwGettersSettersDocMacro(InstitutionName, institutName, std::string, Institution where the equipment that produced the composite instances is located.);

    GDCMIO_API fwGettersSettersDocMacro(ReferencedSOPInstanceUIDs, referencedSOPInstanceUIDs, std::vector< std::string >, The array of SOP Instance UIDs);

    GDCMIO_API void addReferencedSOPInstanceUID(const std::string & a_UID);

    GDCMIO_API fwGettersSettersDocMacro(IsMultiFrame, isMultiFrame, bool, Flag on multi-frame state of an image series);

private:

//    static DicomInstanceWriter * m_unique_instance;

    //** Members which define SOP UID **//
    std::string m_SOPClassUID;          ///< Defined storage type (eg : CT Image Storage, ...).
    std::string m_SOPInstanceUID;       ///< Defined storage instance.

    //** Members which define Instance UID **//
    std::string m_studyInstanceUID;     ///< UID of the current study.
    std::string m_seriesInstanceUID;    ///< UID of the current series (= Acquisition).

    std::string m_instanceNumber;       ///< Number of a storage corresponding to a given series.
                                        ///< (eg : 0,1, ...., 42, ...)

    std::string m_modality;             ///< Modality of the instance.

    std::string m_institutName;         ///< Hospital name.

    //** Members which define IMAGE Instance **//
    std::string m_frameOfRefUID;        ///< UID of the image first frame (see : Tag(0020,0052) Frame of Reference UID )

    std::vector< std::string > m_referencedSOPInstanceUIDs; ///< SOP Instance UID of each frame of an 3D image

    bool        m_isMultiFrame;         ///< Define if the image is multiframe.
};

}

#endif /* _GDCMIO_DICOMINSTANCE_HPP_ */
