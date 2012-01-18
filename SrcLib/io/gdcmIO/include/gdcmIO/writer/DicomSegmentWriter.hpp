/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSEGMENTWRITER_HPP_
#define _GDCMIO_DICOMSEGMENTWRITER_HPP_

#include <gdcmSurfaceWriter.h>

#include <fwData/Acquisition.hpp>
#include <fwData/StructureTraits.hpp>

#include "gdcmIO/writer/DicomRefInstanceWriter.hxx"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class define a writer of segmentation.
 *
 * @note    Currently, it just handles segment writing for a
 *          surface segmentation storage.
 *
 * @class   DicomSegmentWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSegmentWriter : public DicomRefInstanceWriter< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomSegmentWriter();

    GDCMIO_API virtual ~DicomSegmentWriter();

protected :

    /**
     * @brief   Write Surface Segmentation Module thanks to a gdcm::SurfaceWriter.
     *
     * @see     PS 3.3 C.8.23
     *
     * @param   a_gSurfaceWriter   A surface and segment writer.
     */
    void writeSurfaceSegmentation(const unsigned int idx, ::fwData::StructureTraits::csptr infoSegement) throw (::fwTools::Failed);

    /**
     * @brief   Set the index of the segment.
     */
    void setSegmentNumber(const unsigned short a_num);

private :

    unsigned short  m_segmentNumber;    ///< The segment number (eg : 0, 1, ...)

};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSEGMENTWRITER_HPP_ */
