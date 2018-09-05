/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arDataCamp/autoload.hpp"

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>


namespace arDataCamp
{

//Force link with fwDataCamp
static const int fwDataVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;

//Force link with fwMedDataCamp
static const int fwMedDataVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;

runner runner::r;

}
