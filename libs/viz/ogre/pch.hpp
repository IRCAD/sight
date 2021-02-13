/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#ifndef __FWRENDEROGRE_PCH_HPP__
#define __FWRENDEROGRE_PCH_HPP__

#ifndef _WIN32
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/HasSlots.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#endif // _WIN32

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <filesystem>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <Ogre.h>

#endif // __FWRENDEROGRE_PCH_HPP__
