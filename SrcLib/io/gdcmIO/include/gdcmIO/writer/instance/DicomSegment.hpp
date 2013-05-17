/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_INSTANCE_DICOMSEGMENT_HPP__
#define __GDCMIO_WRITER_INSTANCE_DICOMSEGMENT_HPP__

#include <gdcmSurfaceWriter.h>

#include <fwMedData/ModelSeries.hpp>
#include <fwData/StructureTraits.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/instance/DicomInstance.hxx"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

/**
 * @brief   This class define a writer of segmentation.
 *
 * @note    Currently, it just handles segment writing for a
 *          surface segmentation storage.
 *
 * @class   DicomSegment
 */
class GDCMIO_CLASS_API DicomSegment : public DicomInstance< ::fwMedData::ModelSeries >
{

public :

    GDCMIO_API DicomSegment();

    GDCMIO_API virtual ~DicomSegment();

protected :

    /**
     * @brief   Write Surface Segmentation Module thanks to a gdcm::SurfaceWriter.
     *
     * @see     PS 3.3 C.8.23
     *
     * @param   idx  Reconstruction index in acquisition
     * @param infoSegement Segment information
     */
    void writeSurfaceSegmentation(const unsigned int idx,
                                  ::fwData::StructureTraits::csptr infoSegement) throw (::gdcmIO::exception::Failed);

    /**
     * @brief   Set the index of the segment.
     */
    void setSegmentNumber(const unsigned short a_num);

private :

    unsigned short  m_segmentNumber;    ///< The segment number (eg : 0, 1, ...)

};

} // namespace instance
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_INSTANCE_DICOMSEGMENT_HPP__ */
