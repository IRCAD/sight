/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwDcmtkIO/helper/Codec.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

#include <dcmtk/dcmdata/dcrledrg.h>

// Comment to prevent sheldon from sorting the two headers below,
// because they should be included this way around
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmjpeg/dipijpeg.h>

namespace fwDcmtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

fwDcmtkIO::helper::Codec::Codec()
{
    // Register RLE decompression codecs
    DcmRLEDecoderRegistration::registerCodecs();

    // Register JPEG decompression codecs
    DJDecoderRegistration::registerCodecs();
}

//------------------------------------------------------------------------------

fwDcmtkIO::helper::Codec::~Codec()
{
    // Deregister RLE decompression codecs
    DcmRLEDecoderRegistration::cleanup();

    // Deregister JPEG decompression codecs
    DJDecoderRegistration::cleanup();
}

//------------------------------------------------------------------------------

SPTR(::fwDcmtkIO::helper::Codec) g_codecInstantiator =
    ::fwCore::util::LazyInstantiator< ::fwDcmtkIO::helper::Codec >::getInstance();

//------------------------------------------------------------------------------

} //helper
} //fwDcmtkIO
