/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __ARDATACAMP_CAMERASERIES_HPP__
#define __ARDATACAMP_CAMERASERIES_HPP__

#include "arDataCamp/config.hpp"

#include <fwCamp/macros.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

fwCampDeclareAccessor((arData)(CameraSeries), (arData)(Camera));
fwCampDeclareAccessor((arData)(CameraSeries), (fwData)(TransformationMatrix3D));

#endif /* __ARDATACAMP_CAMERASERIES_HPP__ */
