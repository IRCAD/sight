/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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
 * @brief   This class define a container of distances to translate
 * in DICOM/FW4SPL form.
 * @class   DicomDistance
 */
class GDCMIO_CLASS_API DicomDistance
{

public :

    typedef std::vector< ::gdcmIO::container::DicomSCoord > SCoordsType;

    GDCMIO_API DicomDistance();

    GDCMIO_API ~DicomDistance();

    /**
     * @brief   Set distances data in an image.
     *
     * @param   a_image Image where distances could be inserted.
     */
    GDCMIO_API void convertToData(::fwData::Image::sptr a_image);

    /**
     * @brief   Get distances data from an image and convert it to a closed DICOM form.
     *
     * @param   a_image Image which can contain distances.
     */
    GDCMIO_API void setFromData(::fwData::Image::csptr a_image) throw (::gdcmIO::exception::Failed);

    GDCMIO_API fwGettersSettersDocMacro(SCoords, SCoords, SCoordsType, Spatial coordinates container (eg : coordinates on x, y axis));

    GDCMIO_API fwGettersSettersDocMacro(Dists, dists, std::vector<std::string>, Distance values container (e.g: distance value for each distance: "301.256"));

    GDCMIO_API fwGettersSettersDocMacro(RefFrames, refFrames, std::vector<std::string>, SSlice numbers container (equal to coordinates on z axis of 2 points));

    /**
     * @brief   Add a distance value in the container.
     *
     * @param   a_dist  Distance value.
     */
    GDCMIO_API void addDist(const std::string & a_dist);

    /**
     * @brief   Add coordinates for one distance in the container.
     *
     * @param   a_scoord Coordinates of 2 points.
     */
    GDCMIO_API void addSCoord(const ::gdcmIO::container::DicomSCoord & a_scoord);

    /**
     * @brief   Add slice numbers for one distance in the container.
     *
     * @param   a_refFrame  Slice numbers of 2 points.
     */
    GDCMIO_API void addRefFrame(const std::string & a_refFrame);

private :

    SCoordsType                m_SCoords;      ///< Coordinates container (equal to coordinates on x, y axis of 2 points).
                                                ///< (e.g: coordinates for each distance : "x1\y1\x2\y2").

    std::vector<std::string>    m_dists;        ///< Distance values container (e.g: distance value for each distance: "301.256").

    std::vector<std::string>    m_refFrames;    ///< Slice numbers container (equal to coordinates on z axis of 2 points).
                                                ///< (e.g: Slice numbers for each distance: "4\205").
};

} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMDISTANCE_HPP__ */
