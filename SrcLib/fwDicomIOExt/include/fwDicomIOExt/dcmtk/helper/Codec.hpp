/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_CODEC_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_CODEC_HPP__

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @class Codec
 * @brief Dicom Codec Helper. This class is used to load Dicom codecs.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API Codec
{
public:
    /// Load DICOM codec
    FWDICOMIOEXT_API static void registerCodecs();

    /// Clean up codec register
    FWDICOMIOEXT_API static void cleanup();


};

} //dcmtk
} //helper
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_HELPER_CODEC_HPP__ */
