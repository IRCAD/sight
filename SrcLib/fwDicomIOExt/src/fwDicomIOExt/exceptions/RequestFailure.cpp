/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOExt/exceptions/RequestFailure.hpp"

namespace fwDicomIOExt
{
namespace exceptions
{

RequestFailure::RequestFailure(const std::string& message) :
        ::fwDicomIOExt::exceptions::Base(message)
{
}

// ----------------------------------------------------------------------------

RequestFailure::~RequestFailure() throw()
{
}

} //exceptions
} //fwDicomIOExt
