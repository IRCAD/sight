/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSURFACEWRITER_HPP_
#define _GDCMIO_DICOMSURFACEWRITER_HPP_

#include "gdcmIO/writer/DicomSegmentWriter.hpp"
#include "gdcmIO/DicomSurface.hpp"

namespace gdcmIO
{

namespace writer
{

/**
 * @brief   This class define a writer of surface segmentation.
 *
 * It will write one DICOM file to save fwData::Reconstruction.
 * For that, it calls DicomSegmentWriter, defines surface modules
 * and complete the DICOM file.
 *
 * @class   DicomSurfaceWriter
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSurfaceWriter : public DicomSegmentWriter
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::SurfaceWriter.
     */
    GDCMIO_API DicomSurfaceWriter();

    GDCMIO_API ~DicomSurfaceWriter();

    /**
     * @brief   Write surface segmentation in one file.
     */
    GDCMIO_API void write() throw (::fwTools::Failed);

    /**
     * @brief   Set the index of surface to save
     */
    GDCMIO_API void setSurfaceNumber(const unsigned short a_num);

private :

    /**
     * @brief   Write Surface Mesh Module thanks to a gdcm::SurfaceWriter.
     *
     * @see     PS 3.3 C.27.1
     *
     * @param   a_idx   Index of the surface to save.
     */
    void writeSurfaceMesh(const unsigned int a_idx);

    /**
     * @brief   Write Content Identification Module.
     *
     * @see     PS 3.3 C.10.9
     */
    void writeContentIdentification();


    ::boost::shared_ptr<DicomSurface>   m_surface;          ///< Prepare one surface to be written

    unsigned short                      m_surfaceNumber;    ///< The current surface index (eg : 0, 1, ...)

};

}   // namespace writer

}   // namespace gdcmIO

#endif /* _GDCMIO_DICOMSURFACEWRITER_HPP_ */
