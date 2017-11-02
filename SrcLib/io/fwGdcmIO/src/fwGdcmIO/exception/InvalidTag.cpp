/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/exception/InvalidTag.hpp"

namespace fwGdcmIO
{
namespace exception
{

//----------------------------------------------------------------------------

InvalidTag::InvalidTag(const std::string& message) :
    ::fwCore::Exception(message)
{
}

//----------------------------------------------------------------------------

InvalidTag::~InvalidTag() noexcept
{
}

//----------------------------------------------------------------------------

} // namespace exception
} // namespace fwGdcmIO

