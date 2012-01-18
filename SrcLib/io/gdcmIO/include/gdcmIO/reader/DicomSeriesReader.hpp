/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_DICOMSERIESREADER_HPP_
#define _GDCMIO_DICOMSERIESREADER_HPP_

#include <fwData/Acquisition.hpp>

#include "gdcmIO/reader/DicomFilesReader.hxx"

namespace gdcmIO
{

namespace reader
{

/**
 * @brief   This class read a complete tree of a series from DICOM file.
 *
 * It will sort content of files, launch corresponding reading tool and read
 * series information.
 *
 * @class   DicomSeriesReader
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSeriesReader : public DicomFilesReader< ::fwData::Acquisition >
{

public :

    GDCMIO_API DicomSeriesReader();

    GDCMIO_API ~DicomSeriesReader();

    /**
     * @brief   Read one series from DICOM file.
     *
     * It will load then start image, SR and RT reading tools.
     * If any image was found, the reading is stopped.
     *
     * @note    setFileNames() and setObject() have to be called before this method.
     */
    GDCMIO_API void read() throw(::fwTools::Failed);

private :

    /**
     * @brief   Set a series from a DICOM file.
     *
     *@note     setReader() have to be called before this method.
     *          In addition, the reader have to be a gdcm::ImageReader
     */
    void readSeries();

    /**
     * @brief   Get file names which contain DICOM image.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    std::vector< std::string > getImageFileNames(const std::map< std::string, std::vector< std::string > > a_map);

    /**
     * @brief   Get file names which contain DICOM SR document.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    std::vector< std::string > getSRFileNames(const std::map< std::string, std::vector< std::string > > a_map);

    /**
     * @brief   Get file names which contain DICOM RT document.
     *
     * @param   a_map   Map with SOP Class UID in key and file names for value.
     *
     * @return  File names array
     */
    std::vector< std::string > getRTFileNames(const std::map< std::string, std::vector< std::string > > a_map);

};

}  // namespace reader

}  // namespace gdcmIO

#endif /*_GDCMIO_DICOMSERIESREADER_HPP_*/
