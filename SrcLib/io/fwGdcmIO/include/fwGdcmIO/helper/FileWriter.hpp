/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_FILEWRITER_HPP__
#define __FWGDCMIO_HELPER_FILEWRITER_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>

#include <gdcmDataSet.h>
#include <gdcmWriter.h>

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief Helper used to write a DICOM file.
 */
class FWGDCMIO_CLASS_API FileWriter
{

public:
    /**
     * @brief Write a file from the dataset
     * @brief[in] filename Filename
     * @brief[in] writer GDCM writer
     */
    FWGDCMIO_API static void write(const std::string& filename, SPTR(::gdcm::Writer) writer);
};

} // namespace helper
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_HELPER_FILEWRITER_HPP__ */
