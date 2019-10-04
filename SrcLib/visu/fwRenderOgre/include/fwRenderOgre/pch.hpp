/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

// We need a specific pch because we need OpenMP support
// Compile flags will be automatically forwarded so that we get a pch with OpenMP inside

// Cause an internal compiler error on windows...
#pragma once

#ifndef _WIN32
#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#endif // _WIN32

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <Ogre.h>
