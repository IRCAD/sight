/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/Version.hpp>

#include "fwDicomDataCamp/autoload.hpp"

namespace fwDicomDataCamp
{
// Force link with fwDataCamp
const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;

runner runner::r;
}
