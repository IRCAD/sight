/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMRTREADER_HPP_
#define _GDCMIO_DICOMRTREADER_HPP_

#include <fwData/Acquisition.hpp>

#include "gdcmIO/reader/DicomFilesReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class define a reader of RT document from DICOM file.
 *
 * @note    Currently, it just handles surface segmentation storage.
 *
 * @class   DicomRTReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomRTReader : public DicomFilesReader< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomRTReader();

    GDCMIO_API ~DicomRTReader();

    /**
     * @brief   Read RT document from DICOM file.
     *
     * @note    Currently, it just handles surface segmentation reading.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw (::fwTools::Failed);

private :

    /**
     * @brief   Get file names which contain surface segmentation storage.
     *
     * @param   a_gScanner  Scanner which searched surface segmentation storage.
     */
    std::vector< std::string > getSurfaceStorageFileNames( ::gdcm::Scanner const & a_gScanner );

};

}  // namespace reader

}  // namespace gdcmIO

#endif /* _GDCMIO_DICOMRTREADER_HPP_ */
