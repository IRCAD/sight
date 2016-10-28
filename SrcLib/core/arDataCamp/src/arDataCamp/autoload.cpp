/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/Version.hpp>

#include "arDataCamp/autoload.hpp"


namespace arDataCamp
{

//Force link with fwDataCamp
static const int fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

runner runner::r;

}
