/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATACAMP_DICOMSERIES_HPP__
#define __FWMEDDATACAMP_DICOMSERIES_HPP__

#include "fwMedDataCamp/config.hpp"

#include <fwCamp/macros.hpp>

#include <fwData/Array.hpp>

#include <fwMedData/DicomSeries.hpp>

fwCampAutoDeclareEnumMacro((fwMedData)(DicomSeries)(DICOM_AVAILABILITY));

fwCampDeclareAccessor((fwMedData)(DicomSeries), (fwData)(Array));

#endif // __FWMEDDATACAMP_DICOMSERIES_HPP__
