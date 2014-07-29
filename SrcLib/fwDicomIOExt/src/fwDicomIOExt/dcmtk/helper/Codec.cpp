/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/dipijpeg.h>

#include "fwDicomIOExt/dcmtk/helper/Codec.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

void Codec::registerCodecs()
{
    // Register RLE decompression codecs
    DcmRLEDecoderRegistration::registerCodecs();

    // Register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs();
}

// ----------------------------------------------------------------------------

void Codec::cleanup()
{
    // Deregister RLE decompression codecs
    DcmRLEDecoderRegistration::cleanup();

    // Deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();
}


} //dcmtk
} //helper
} //fwDicomIOExt
