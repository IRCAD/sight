/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwMedData/ActivitySeries.hpp"
#include "fwMedData/detail/reflection/ActivitySeries.hpp"
#include "fwMedData/detail/reflection/DicomSeries.hpp"
#include "fwMedData/detail/reflection/ImageSeries.hpp"
#include "fwMedData/detail/reflection/Series.hpp"
#include "fwMedData/DicomSeries.hpp"
#include "fwMedData/Equipment.hpp"
#include "fwMedData/ImageSeries.hpp"
#include "fwMedData/ModelSeries.hpp"
#include "fwMedData/NavigationSeries.hpp"
#include "fwMedData/Patient.hpp"
#include "fwMedData/Series.hpp"
#include "fwMedData/SeriesDB.hpp"
#include "fwMedData/Study.hpp"

#include <fwCamp/macros.hpp>

namespace fwMedData
{
namespace detail
{
namespace reflection
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
        localDeclarefwMedDataDicomSeries();
        localDeclarefwMedDataImageSeries();
        localDeclarefwMedDataModelSeries();
        localDeclarefwMedDataActivitySeries();
        localDeclarefwMedDataNavigationSeries();
    }

    static runner r;
};

} // namespace reflection
} // namespace detail
} // namespace fwMedData
