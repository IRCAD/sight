/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_HELPER_CODEC_HPP__
#define __FWDCMTKIO_HELPER_CODEC_HPP__

#include "fwDcmtkIO/config.hpp"

namespace fwDcmtkIO
{
namespace helper
{

/**
 * @brief Dicom Codec Helper. This class is used to load Dicom codecs.
 */
class FWDCMTKIO_CLASS_API Codec
{
public:
    /// Load DICOM codec
    FWDCMTKIO_API static void registerCodecs();

    /// Clean up codec register
    FWDCMTKIO_API static void cleanup();


};

} //helper
} //fwDcmtkIO


#endif /* __FWDCMTKIO_HELPER_CODEC_HPP__ */
