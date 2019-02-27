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

#pragma once

#include "fwDcmtkIO/config.hpp"

#include <fwCore/macros.hpp>
#include <fwCore/spyLog.hpp>

#include <boost/noncopyable.hpp>

namespace fwDcmtkIO
{
namespace helper
{

/**
 * @brief Dicom Codec Helper. This class is used to load decompression codecs.
 */
class FWDCMTKIO_CLASS_API Codec : public ::boost::noncopyable
{
public:
    /// Load decompression codecs
    Codec();
    /// Clean up codecs
    ~Codec();

    /// @deprecated This function should not be used anymore and will be removed in 19.0
    FWDCMTKIO_API static void registerCodecs()
    {
        FW_DEPRECATED_MSG("This function is deprecated.", "19.0");
    }

    /// @deprecated This function should not be used anymore and will be removed in 19.0
    FWDCMTKIO_API static void cleanup()
    {
        FW_DEPRECATED_MSG("This function is deprecated.", "19.0");
    }
};

extern FWDCMTKIO_CLASS_API SPTR(::fwDcmtkIO::helper::Codec) g_codecInstantiator;

} //helper

} //fwDcmtkIO
