/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMSEGMENT_HPP__
#define __GDCMIO_READER_DICOM_DICOMSEGMENT_HPP__

#include <gdcmSegment.h>

#include <fwData/Reconstruction.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/IDicomFiles.hxx"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
* @brief   This class implement a segmentation reader.
*
* @note    Currently, it just handles segmentation from surface segmentation storage.
*
* @class   DicomSegment
*/
class GDCMIO_CLASS_API DicomSegment : public IDicomFiles< ::fwMedData::ModelSeries >
{

public :

   GDCMIO_API DicomSegment();

   GDCMIO_API virtual ~DicomSegment();

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
                                         ::gdcm::SmartPointer< ::gdcm::Segment >   a_segment ) throw (::gdcmIO::exception::Failed);

private :

   /**
    * @brief   Check if segmentation has surface.
    */
   bool isSurfaceSegmentation();

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMSEGMENT_HPP__ */
