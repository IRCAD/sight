/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMACTIVITIESCAMP_AUTOLOAD_HPP__
#define __FWMACTIVITIESCAMP_AUTOLOAD_HPP__

#include <fwCamp/macros.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include "fwMedDataCamp/Series.hpp"


namespace fwActivitiesCamp
{

struct runner
{
    runner()
    {
        localDeclarefwMedDataActivitySeries();
    }

    static runner r;
};

} //end namespace fwActivitiesCamp

#endif //__FWMACTIVITIESCAMP_AUTOLOAD_HPP__
