/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomDataCamp/DicomSeries.hpp"

#include <fwCamp/macros.hpp>
#include <fwDataCamp/Version.hpp>
#include <fwDataCamp/visitor/CompareObjects.hpp>
#include <fwDicomData/DicomSeries.hpp>



namespace fwDicomDataCamp
{

struct runner
{
    runner()
    {
        // Effective force link with fwDataCamp
        ::fwDataCamp::visitor::CompareObjects comparator;
        comparator.getReferenceObject();

        localDeclarefwDicomDataDicomSeriesDICOM_AVAILABILITY();
        localDeclarefwDicomDataDicomSeries();
    }

    static runner r;

};

runner runner::r;

} //namespace fwDicomDataCamp

