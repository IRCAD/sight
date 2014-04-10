/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMDISTANCE_HPP__
#define __GDCMIO_CONTAINER_DICOMDISTANCE_HPP__

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
 * @brief This class defines a container of distances to translate into DICOM/FW4SPL form.
 * @class DicomDistance
 */
class GDCMIO_CLASS_API DicomDistance
{

public:

    typedef std::vector< ::gdcmIO::container::DicomSCoord > SCoordContainerType;
    typedef std::vector<std::string> DistanceValueContainerType;
    typedef std::vector<std::string> SliceNumberContainerType;

    /// Constructor
    GDCMIO_API DicomDistance();

    /// Destructor
    GDCMIO_API ~DicomDistance();

    /**
     * @brief Add a distance value in the container.
     * @param[in] distance Distance value.
     * @param[in] scoord Coordinates of 2 points.
     * @param[in] referencedFrame Slice numbers of 2 points.
     */
    GDCMIO_API void addDistance(const std::string &distance, const ::gdcmIO::container::DicomSCoord &scoord,
            const std::string &referencedFrame);

    /**
     * @brief Get distances data from an image and convert it to a closed DICOM form.
     * @param[in] image Image which can contain distances.
     */
    GDCMIO_API void setFromData(::fwData::Image::csptr image) throw (::gdcmIO::exception::Failed);

    /**
     * @brief Set distances data in an image.
     * @param[in] image Image where distances could be inserted.
     */
    GDCMIO_API void convertToData(::fwData::Image::sptr image);


    GDCMIO_API fwGettersSettersDocMacro(SCoordContainer, scoordContainer, SCoordContainerType,
            Spatial coordinates container (eg : coordinates on x, y axis));

    GDCMIO_API fwGettersSettersDocMacro(DistanceContainer, distanceContainer, DistanceValueContainerType,
            Distance values container (e.g: distance value for each distance: "301.256"));

    GDCMIO_API fwGettersSettersDocMacro(ReferencedFrameContainer, referencedFrameContainer, SliceNumberContainerType,
            SSlice numbers container (equal to coordinates on z axis of 2 points));


private :

    /// Coordinates container (equal to coordinates on x, y axis of 2 points).
    /// (e.g: coordinates for each distance : "x1\y1\x2\y2").
    SCoordContainerType m_scoordContainer;

    /// Distance values container (e.g: distance value for each distance: "301.256").
    DistanceValueContainerType m_distanceContainer;

    /// Slice numbers container (equal to coordinates on z axis of 2 points).
    /// (e.g: Slice numbers for each distance: "4\205").
    SliceNumberContainerType m_referencedFrameContainer;
};

}
 //namespace container
}//namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMDISTANCE_HPP__ */
