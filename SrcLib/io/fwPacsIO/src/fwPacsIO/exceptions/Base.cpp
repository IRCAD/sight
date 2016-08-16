/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPacsIO/exceptions/Base.hpp"

namespace fwPacsIO
{
namespace exceptions
{

Base::Base(const std::string& message) : ::fwCore::Exception(message)
{
}

// ----------------------------------------------------------------------------

Base::~Base() throw()
{
}

} //exceptions
} //fwPacsIO
