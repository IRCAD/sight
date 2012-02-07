/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSURFACE_HPP_
#define _GDCMIO_DICOMSURFACE_HPP_

#include <stdint.h> // for uint16_t on Windows

#include <fwData/Reconstruction.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{

/**
 * @brief   This class define one surface mesh item in order to translate
 * in DICOM/FW4SPL form.
 *
 * @class   DicomSurface
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSurface
{

public :

    GDCMIO_API DicomSurface();

    GDCMIO_API ~DicomSurface();

    /**
     * @brief   Set members from a reconstruction.
     *
     * @param   a_reconstruction    Source reconstruction
     */
    GDCMIO_API void setFromData(fwData::Reconstruction::csptr a_reconstruction);

    /**
     * @brief   Set a reconstruction from DicomSurface data.
     *
     * @note    All members have to be set before.
     *
     * @param   a_reconstruction    Destination reconstruction
     */
    GDCMIO_API void convertToData(fwData::Reconstruction::sptr a_reconstruction);

    GDCMIO_API ::fwData::Mesh::sptr convertToData(const float *coord, const unsigned long coordSize,
                                                  const uint32_t *index, const unsigned long indexSize,
                                                  const float * normalCoord);

    GDCMIO_API const ::boost::shared_ptr<float>     getPointCoordData() const;
    GDCMIO_API unsigned long                        getPointCoordSize() const;

    GDCMIO_API const ::boost::shared_ptr<uint32_t>  getPointIndexList() const;
    GDCMIO_API unsigned long                        getPointIndexSize() const;

    GDCMIO_API const ::boost::shared_ptr<float>     getNormalCoordData() const;
    GDCMIO_API unsigned long                        getNormalCoordSize() const;

    GDCMIO_API void setPointCoordData(const ::boost::shared_ptr<float>  a_array);
    GDCMIO_API void setPointCoordSize(const unsigned long               a_size);

    GDCMIO_API void setPointIndexList(const ::boost::shared_ptr<uint32_t>   a_array);
    GDCMIO_API void setPointIndexSize(const unsigned long                   a_size);

    GDCMIO_API void setNormalCoordData(const ::boost::shared_ptr<float>  a_array);
    GDCMIO_API void setNormalCoordSize(const unsigned long               a_size);

private :

    //** Data for one surface **//
    ::boost::shared_ptr<float>      m_pointCoordData;   ///< Surface Points Coordinates (List of points coordinates for one surface)
                                                        ///< (eg : x1,y1,z1, x2,y2,z2, ...).

    unsigned long                   m_pointCoordSize;   ///< Number of points.

//    ::boost::shared_ptr<uint16_t>   m_pointIndexList;   ///< Surface Mesh Primitives (List of points index for all triangle of one surface)
//                                                        ///< (eg : p1,p3,p2, p2,p3,p4, ...).
    // Primitives will be written in Triangle Point Index List ( Tag(0066,0023) ) which has a VR equal to OW
    // VR::OW is a string of 16-bit words where the encoding of the contents is specified by the negotiated Transfer Syntax
    // Here, we try to force VR::OW to have a length of 32 bits for each primitive
    ::boost::shared_ptr<uint32_t>   m_pointIndexList;   ///< Surface Mesh Primitives (List of points index for all triangle of one surface)
                                                            ///< (eg : p1,p3,p2, p2,p3,p4, ...).

    unsigned long                   m_pointIndexSize;   ///< Number of primitives/cells.

    ::boost::shared_ptr<float>      m_normalCoordData;   ///< Surface Point Normal Coordinates (List of point normal coordinates for one surface)
                                                              ///< (eg : x1,y1,z1, x2,y2,z2, ...).

    unsigned long                   m_normalCoordSize;   ///< Number of normals.
};

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMSURFACE_HPP_ */
