/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMSURFACE_HPP__
#define __GDCMIO_READER_DICOM_DICOMSURFACE_HPP__

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/DicomSegment.hpp"
#include "gdcmIO/container/DicomSurface.hpp"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
* @brief   This class define a reader of surface segmentation storage.
*
* It will create a new fwData::Reconstruction for one segmentation.
*
* @class   DicomSurface
*/
class GDCMIO_CLASS_API DicomSurface : public DicomSegment
{

public :

   /**
    * @brief   Constructor which instantiate a gdcm::SurfaceReader.
    */
   GDCMIO_API DicomSurface();

   GDCMIO_API virtual ~DicomSurface();

   /**
    * @brief   Read surface segmentation from a DICOM file.
    */
   GDCMIO_API void read() throw (::gdcmIO::exception::Failed);

private :

   /**
    * @brief   Read surface sequence.
    *
    * @see     DICOM Supplement 132
    *
    * @param   a_surface   Item of Surface Sequence to read.
    */
   bool readSurface(const ::gdcm::Item & a_surface) throw (::gdcmIO::exception::Failed);

   /**
    * @brief   Launch all surface segmentation reading.
    *
    * @see     DICOM Supplement 132
    */
   void readSurfaceMeshs() throw (::gdcmIO::exception::Failed);

   /**
    * @brief   Read surface mesh module and launch surface segmentation module reading.
    *
    * @see     PS 3.3 C.27.1 and C.8.23.1
    *
    * @param   a_segment   Segmentation which contains surface.
    */
   void readSurfaceMesh( ::gdcm::SmartPointer< ::gdcm::Segment > a_segment) throw (::gdcmIO::exception::Failed);


   ::boost::shared_ptr< ::gdcmIO::container::DicomSurface > m_surfaces; ///< Object which convert one DICOM surface into FW4SPL.

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMSURFACE_HPP__ */

