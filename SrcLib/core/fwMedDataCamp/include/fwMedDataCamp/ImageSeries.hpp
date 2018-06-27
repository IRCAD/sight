/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwMedDataCamp/config.hpp"

#include <fwCamp/macros.hpp>

#include <fwData/Image.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>

fwCampDeclareAccessor((fwMedData)(ImageSeries), (fwData)(Image));
fwCampDeclareAccessor((fwMedData)(ImageSeries), (fwMedData)(DicomSeries));
