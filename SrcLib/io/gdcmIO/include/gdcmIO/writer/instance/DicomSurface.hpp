/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_WRITER_INSTANCE_DICOMSURFACE_HPP__
#define __GDCMIO_WRITER_INSTANCE_DICOMSURFACE_HPP__

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/writer/instance/DicomSegment.hpp"
#include "gdcmIO/container/DicomSurface.hpp"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

/**
 * @brief   This class define a writer of surface segmentation.
 *
 * It will write one DICOM file to save fwData::Reconstruction.
 * For that, it calls DicomSegment, defines surface modules
 * and complete the DICOM file.
 *
 * @class   DicomSurface
 */
class GDCMIO_CLASS_API DicomSurface : public DicomSegment
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::SurfaceWriter.
     */
    GDCMIO_API DicomSurface();

    GDCMIO_API ~DicomSurface();

    /**
     * @brief   Write surface segmentation in one file.
     */
    GDCMIO_API void write() throw (::gdcmIO::exception::Failed);

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


    /// Prepare one surface to be written
    ::boost::shared_ptr< ::gdcmIO::container::DicomSurface >   m_surface;

    unsigned short m_surfaceNumber;    ///< The current surface index (eg : 0, 1, ...)

};

} // namespace instance
} // namespace writer
} // namespace gdcmIO

#endif /* __GDCMIO_WRITER_INSTANCE_DICOMSURFACE_HPP__ */
