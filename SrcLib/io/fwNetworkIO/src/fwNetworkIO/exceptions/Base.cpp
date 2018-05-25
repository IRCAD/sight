/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwNetworkIO/exceptions/Base.hpp"

namespace fwNetworkIO
{

namespace exceptions
{

Base::Base(const std::string& message) :
    ::fwCore::Exception(message)
{
}

Base::~Base() noexcept
{
}

} // namespace exceptions
} // namespace fwNetworkIO
