/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOExt/exceptions/Base.hpp"

namespace fwDicomIOExt
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
} //fwDicomIOExt
