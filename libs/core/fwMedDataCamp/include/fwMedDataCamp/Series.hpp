/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWMEDDATACAMP_SERIES_HPP__
#define __FWMEDDATACAMP_SERIES_HPP__

#include "fwMedDataCamp/config.hpp"

#include <fwCamp/macros.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

fwCampDeclareAccessor((fwMedData)(Series), (fwMedData)(Patient));
fwCampDeclareAccessor((fwMedData)(Series), (fwMedData)(Study));
fwCampDeclareAccessor((fwMedData)(Series), (fwMedData)(Equipment));

#endif /* __FWMEDDATACAMP_SERIES_HPP__ */
