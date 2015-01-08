/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATACAMP_MATERIAL_HPP__
#define __FWDATACAMP_MATERIAL_HPP__

#include <fwCamp/macros.hpp>
#include <fwData/Material.hpp>

#include "fwDataCamp/config.hpp"

fwCampAutoDeclareEnumMacro((fwData)(Material)(SHADING_MODE));
fwCampAutoDeclareEnumMacro((fwData)(Material)(REPRESENTATION_MODE));
fwCampAutoDeclareEnumMacro((fwData)(Material)(OPTIONS_MODE));

fwCampDeclareAccessor((fwData)(Material), (fwData)(Color));

#endif /* __FWDATACAMP_MATERIAL_HPP__ */
