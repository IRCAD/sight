/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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
