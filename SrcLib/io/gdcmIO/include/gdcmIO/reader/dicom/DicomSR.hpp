/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMSR_HPP__
#define __GDCMIO_READER_DICOM_DICOMSR_HPP__

#include <fwData/Image.hpp>

#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/reader/dicom/IDicomInstance.hxx"
#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

/**
 * @brief   This class define a reader of SR (Structured Report) document.
 *
 * It will launch specific SR readers on a same DICOM file.
 *
 * @class   DicomSR
 */
class GDCMIO_CLASS_API DicomSR : public IDicomInstance< ::fwData::Image >
{

public :

    /**
     * @brief   Constructor which instantiate a gdcm::Reader.
     */
    GDCMIO_API DicomSR();

    GDCMIO_API ~DicomSR();

    /**
     * @brief   Read one document SR.
     *
     * @note    Currently, it just handles landmarks and distances reading.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::gdcmIO::exception::Failed);

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMSR_HPP__ */
