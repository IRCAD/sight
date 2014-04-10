/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMLANDMARK_HPP__
#define __GDCMIO_CONTAINER_DICOMLANDMARK_HPP__

#include <string>
#include <vector>

#include <fwData/Image.hpp>
#include <fwData/macros.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/container/DicomSCoord.hpp"

namespace gdcmIO
{
namespace container
{

/**
 * @class DicomLandmark
 * @brief This class define a container of landmarks to translate in DICOM/FW4SPL form.
 */
class GDCMIO_CLASS_API DicomLandmark
{

public:

    typedef std::vector<std::string> LabelContainerType;
    typedef std::vector< ::gdcmIO::container::DicomSCoord > SCoordContainerType;
    typedef std::vector<unsigned int> FrameNumberContainerType;

    /// Constructor
    GDCMIO_API DicomLandmark();

    /// Destructor
    GDCMIO_API ~DicomLandmark();

    /**
     * @brief Add to DicomLandmark the label of one landmark.
     * @param[in] label Add label to landmark
     * @param[in] point Add coordinate to landmark
     * @param[in] refFrame Add referenced frame for the landmark
     */
    GDCMIO_API void addLandmark(const std::string &label, const ::gdcmIO::container::DicomSCoord &point,
            const unsigned int referencedFrame);

    /**
     * @brief Get landmark(s) data from an image and convert it to a closed DICOM form.
     * @param[in] image Image which can contain landmark(s).
     */
    GDCMIO_API void setFromData(::fwData::Image::csptr image) throw (::gdcmIO::exception::Failed);

    /**
     * @brief Set landmark(s) data in an image.
     * @param[in] image Image where landmark(s) could be inserted.
     */
    GDCMIO_API void convertToData(::fwData::Image::sptr image);


    GDCMIO_API fwGettersSettersDocMacro(LabelContainer, labelContainer, LabelContainerType,
            Labels container (eg : label for each landmark : "Label_1"));

    GDCMIO_API fwGettersSettersDocMacro(SCoordContainer, scoordContainer, SCoordContainerType,
            Spatial coordinates container (eg : coordinates on x, y axis));

    GDCMIO_API fwGettersSettersDocMacro(ReferencedFrameContainer, referencedFrameContainer, FrameNumberContainerType,
            Slice numbers container (equal to coordinate on z axis));

private :

    /// Labels' container (e.g: label for each landmark : "Label_1")
    LabelContainerType m_labelContainer;

    /// Coordinates' container (equal to coordinates on x, y axis). (e.g: coordinates for each landmark : "175.0,23.5").
    SCoordContainerType m_scoordContainer;

    /// Slice numbers' container (equal to coordinate on z axis)
    FrameNumberContainerType m_referencedFrameContainer;
};


} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMLANDMARK_HPP__ */
