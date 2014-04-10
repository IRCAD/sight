/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_HELPER_DICOMSR_HPP__
#define __GDCMIO_HELPER_DICOMSR_HPP__

#include <boost/algorithm/string/trim.hpp>

// GDCM
#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmImage.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmSurface.h>
#include <gdcmString.h>

// fwTools
#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

// fwData
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/container/DicomSCoord.hpp"
#include "gdcmIO/container/DicomCodedAttribute.hpp"

namespace gdcmIO
{

namespace helper
{

/**
 * @class   DicomSR
 * @brief   This class contains helpers for DICOM Structured Reporting (SR).
 */
class GDCMIO_CLASS_API DicomSR
{
public:

    /**
     * @brief Get the data set which contains a coded container.
     * A null pointer means any container was found.
     * @param[in] codeValue Code value to find.
     * @param[in] dataset Data set of a DICOM file.
     * @return Data set pointer.
     */
    GDCMIO_API static const ::gdcm::DataSet* getCodedContainer(const std::string &codeValue,
                                                                const ::gdcm::DataSet & dataset);

    /**
     * @brief Get the data set which contains a typed container.
     * A null pointer means any container was found.
     * @param[in] typeValue Type value to find.
     * @param[in] dataset Data set of a DICOM file.
     * @param[in] index Index of the container
     * @return Data set pointer.
     */
    GDCMIO_API static const ::gdcm::DataSet* getTypedContainer(const std::string &typeValue,
                                                                const ::gdcm::DataSet &dataset,
                                                                unsigned int index = 0);

    /**
     * @brief Create one Spatial coordinate (SCOORD) item in a sequence of items.
     * @see PS 3.3 C.17.3 and C.18.6
     * @param[in] scoord SCOORD to save in data set.
     * @param[in] refFrames Referenced Frame numbers to insert.
     * @param[in] classUID Referenced SOP Class UID.
     * @param[in] instanceUID Referenced SOP Instance UID.
     * @param[in] sequence Sequence of items where new item will be inserted.
     */
    GDCMIO_API static void createSCoord(const ::gdcmIO::container::DicomSCoord &scoord,
                                        const std::string &refFrames,
                                        const std::string &classUID,
                                        const std::string &instanceUID,
                                        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence);
    /**
     * @brief Create one Spatial coordinate (SCOORD) item in a sequence of items.
     * @note This SCOORD refers to an multi-frame image.
     * @param[in] scoord SCOORD to save in data set.
     * @param[in] refFrames Frame indexes of related image (equals to Z coordinates).
     * @param[in] classUID Referenced SOP class UID of the image.
     * @param[in] instanceUID Referenced SOP instance UID of the image.
     * @param[in] dataset Data set where SCOORD node will be written.
     */
    GDCMIO_API static void createSCoord(const ::gdcmIO::container::DicomSCoord &scoord,
                                        const std::string &refFrames,
                                        const std::string &classUID,
                                        const std::string &instanceUID,
                                        ::gdcm::DataSet &dataset);

    /**
     * @brief Create one Spatial coordinate (SCOORD) item in a sequence of items.
     * @note This SCOORD refers to an non multi-frame image.
     * @param[in] scoord SCOORD to save in data set.
     * @param[in] classUID Referenced SOP class UID of the frame.
     * @param[in] instanceUID Referenced SOP instance UID of the frame.
     * @param[in] dataset Data set where SCOORD node will be written.
     */
    GDCMIO_API static void createSCoord(const ::gdcmIO::container::DicomSCoord &scoord,
                                        const std::string &classUID,
                                        const std::string &instanceUID,
                                        ::gdcm::DataSet &dataset);

    /**
     * @brief   Create one IMAGE item in a sequence of items.
     * @note    \p refFrames can be empty. This case is valid
     *          when image is a non multiframe one or all the frames
     *          of the 3D image is referenced.
     * @see     PS 3.3 C.17.3 and C.18.4
     * @param[in] refFrames Referenced frame number.
     * @param[in] classUID Referenced SOP class UID of the frame.
     * @param[in] instanceUID Referenced SOP instance UID of the frame.
     * @param[in] sequence Sequence of items where IMAGE node will be written.
     */
    GDCMIO_API static void createIMAGE(const std::string &refFrames,
                                       const std::string &classUID,
                                       const std::string &instanceUID,
                                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence);

    /**
     * @brief   Convert an referenced SOP instance UID into a frame number.
     * @param[in]   instanceUID   A referenced SOP instance UID.
     * @param[in]   referencedInstanceUIDs    Ordered list of all image SOP Instance UIDs.
     * @return  A frame number.
     */
    GDCMIO_API static unsigned int instanceUIDToFrameNumber(const std::string &instanceUID,
                                                            const std::vector< std::string > &referencedInstanceUIDs) throw(::gdcmIO::exception::Failed);
    /**
     * @brief Read a SCOORD from its data set.
     * @param[in]dataset The data set where the SCOORD is defined.
     * @return A SCoord.
     */
    GDCMIO_API static ::gdcmIO::container::DicomSCoord readSCOORD(const ::gdcm::DataSet & dataset);

};

} // namespace helper

} // namespace gdcmIO

#endif /*__GDCMIO_HELPER_DICOMSR_HPP__*/
