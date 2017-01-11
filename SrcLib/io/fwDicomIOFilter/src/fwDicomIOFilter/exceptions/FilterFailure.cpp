/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"

namespace fwDicomIOFilter
{
namespace exceptions
{

FilterFailure::FilterFailure(const std::string& message) : ::fwCore::Exception(message)
{
}

// ----------------------------------------------------------------------------

FilterFailure::~FilterFailure() throw()
{
}

} // namespace exceptions
} // namespace fwDicomIOFilter
