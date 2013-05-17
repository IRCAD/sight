/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_DICOM_DICOMRT_HPP__
#define __GDCMIO_READER_DICOM_DICOMRT_HPP__

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
 * @brief   This class define a reader of RT document from DICOM file.
 *
 * @note    Currently, it just handles surface segmentation storage.
 *
 * @class   DicomRT
 */
class GDCMIO_CLASS_API DicomRT : public IDicomFiles< ::fwMedData::ModelSeries >
{

public :

    GDCMIO_API DicomRT();

    GDCMIO_API ~DicomRT();

    /**
     * @brief   Read RT document from DICOM file.
     *
     * @note    Currently, it just handles surface segmentation reading.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::gdcmIO::exception::Failed);

private :

    /**
     * @brief   Get file names which contain surface segmentation storage.
     *
     * @param   a_gScanner  Scanner which searched surface segmentation storage.
     */
    std::vector< std::string > getSurfaceStorageFileNames( ::gdcm::Scanner const & a_gScanner );

};

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO

#endif /* __GDCMIO_READER_DICOM_DICOMRT_HPP__ */
