/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOExt/exceptions/TagMissing.hpp"

namespace fwDicomIOExt
{
namespace exceptions
{

TagMissing::TagMissing(const std::string& message) :
        ::fwDicomIOExt::exceptions::Base(message)
{
}

// ----------------------------------------------------------------------------

TagMissing::~TagMissing() throw()
{
}

} //exceptions
} //fwDicomIOExt
