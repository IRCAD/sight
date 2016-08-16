/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/helper/Codec.hpp"

#include <dcmtk/dcmdata/dcrledrg.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/dipijpeg.h>

namespace fwDcmtkIO
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


} //helper
} //fwDcmtkIO
