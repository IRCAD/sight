/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
