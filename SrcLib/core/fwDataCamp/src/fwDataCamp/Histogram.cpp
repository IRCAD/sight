/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Histogram.hpp"

fwCampImplementDataMacro((fwData)(Histogram))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .property("value",&::fwData::Histogram::m_values)
        .property("bins_width",&::fwData::Histogram::m_binsWidth)
        .property("min_value",&::fwData::Histogram::m_minValue)
        .property("max_value",&::fwData::Histogram::m_maxValue)
        ;
}
