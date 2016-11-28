/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
