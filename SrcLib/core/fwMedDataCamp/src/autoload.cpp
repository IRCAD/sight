/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedDataCamp/autoload.hpp"

#include <fwDataCamp/Version.hpp>

namespace fwMedDataCamp
{

//Force link with fwDataCamp
static const int fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

runner runner::r;
}
