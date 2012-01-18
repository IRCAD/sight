/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSURFACEREADER_HPP_
#define _GDCMIO_DICOMSURFACEREADER_HPP_

#include "gdcmIO/reader/DicomSegmentReader.hpp"
#include "gdcmIO/DicomSurface.hpp"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class define a reader of surface segmentation storage.
 *
 * It will create a new fwData::Reconstruction for one segmentation.
 *
 * @class   DicomSurfaceReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSurfaceReader : public DicomSegmentReader
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::SurfaceReader.
     */
    GDCMIO_API DicomSurfaceReader();

    GDCMIO_API virtual ~DicomSurfaceReader();

    /**
     * @brief   Read surface segmentation from a DICOM file.
     */
    GDCMIO_API void read() throw (::fwTools::Failed);

private :

    /**
     * @brief   Read surface sequence.
     *
     * @see     DICOM Supplement 132
     *
     * @param   a_surface   Item of Surface Sequence to read.
     */
    bool readSurface(const ::gdcm::Item & a_surface) throw (::fwTools::Failed);

    /**
     * @brief   Launch all surface segmentation reading.
     *
     * @see     DICOM Supplement 132
     */
    void readSurfaceMeshs() throw (::fwTools::Failed);

    /**
     * @brief   Read surface mesh module and launch surface segmentation module reading.
     *
     * @see     PS 3.3 C.27.1 and C.8.23.1
     *
     * @param   a_segment   Segmentation which contains surface.
     */
    void readSurfaceMesh( ::gdcm::SmartPointer< ::gdcm::Segment > a_segment) throw (::fwTools::Failed);


    ::boost::shared_ptr< DicomSurface > m_surfaces; ///< Object which convert one DICOM surface into FW4SPL.

};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMSURFACEREADER_HPP_ */
