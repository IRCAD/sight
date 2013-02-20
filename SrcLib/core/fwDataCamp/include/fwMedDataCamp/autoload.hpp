/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATACAMP_AUTOLOAD_HPP__
#define __FWMEDDATACAMP_AUTOLOAD_HPP__

#include <fwCamp/macros.hpp>


#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>


namespace fwMedDataCamp
{

struct runner
{
    runner()
    {
        localDeclarefwMedDataEquipment();
        localDeclarefwMedDataStudy();
        localDeclarefwMedDataPatient();
        localDeclarefwMedDataSeries();
        localDeclarefwMedDataSeriesDB();
        localDeclarefwMedDataImageSeries();
        localDeclarefwMedDataModelSeries();
    }

    static runner r;
};

} //end namespace fwMedDataCamp

#endif //__FWMEDDATACAMP_AUTOLOAD_HPP__
