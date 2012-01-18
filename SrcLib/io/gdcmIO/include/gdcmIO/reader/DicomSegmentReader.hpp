/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSEGMENTREADER_HPP_
#define _GDCMIO_DICOMSEGMENTREADER_HPP_

#include <gdcmSegment.h>

#include <fwData/Acquisition.hpp>

#include "gdcmIO/reader/DicomFilesReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class implement a segmentation reader.
 *
 * @note    Currently, it just handles segmentation from surface segmentation storage.
 *
 * @class   DicomSegmentReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSegmentReader : public DicomFilesReader< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomSegmentReader();

    GDCMIO_API virtual ~DicomSegmentReader();

protected :

    /**
     * @brief   Read one surface segmentation module.
     *
     * @see     PS 3.3 C.8.23.1.
     *
     * @param   a_reconstruction    fwData::Reconstruction to set.
     * @param   a_segment           Current segmentation.
     */
    virtual void readSurfaceSegmentation( ::fwData::Reconstruction::sptr            a_reconstruction,
                                          ::gdcm::SmartPointer< ::gdcm::Segment >   a_segment ) throw (::fwTools::Failed);

private :

    /**
     * @brief   Check if segmentation has surface.
     */
    bool isSurfaceSegmentation();

};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMSEGMENTREADER_HPP_ */
