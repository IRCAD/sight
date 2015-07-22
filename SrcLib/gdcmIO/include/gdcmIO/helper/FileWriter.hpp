/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_HELPER_FILEWRITER_HPP__
#define __GDCMIO_HELPER_FILEWRITER_HPP__

#include <gdcmDataSet.h>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace helper
{

/**
 * @class FileWriter.
 * @brief Helper used to write a DICOM file.
 */
class GDCMIO_CLASS_API FileWriter
{

public:
    /**
     * @brief Write a file from the dataset
     * @brief[in] filename Filename
     * @brief[in] writer GDCM writer
     */
    GDCMIO_API static void write(const std::string& filename, SPTR(::gdcm::Writer) writer);
};

} // namespace helper
} // namespace gdcmIO

#endif /* __GDCMIO_HELPER_FILEWRITER_HPP__ */
