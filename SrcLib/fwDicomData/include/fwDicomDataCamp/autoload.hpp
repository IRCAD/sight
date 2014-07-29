/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMDATACAMP_AUTOLOAD_HPP__
#define __FWDICOMDATACAMP_AUTOLOAD_HPP__

#include <fwCamp/macros.hpp>

#include <fwDicomData/DicomSeries.hpp>

#include "fwDicomDataCamp/DicomSeries.hpp"

namespace fwDicomDataCamp
{

struct runner
{
    runner()
    {
        localDeclarefwDicomDataDicomSeriesDICOM_AVAILABILITY();
        localDeclarefwDicomDataDicomSeries();
    }

    static runner r;
};

} //end namespace fwDicomDataCamp

#endif //__FWDICOMDATACAMP_AUTOLOAD_HPP__
