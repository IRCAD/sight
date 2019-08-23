/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#pragma once

#include <fwCore/TimeStamp.hpp>

#include <boost/preprocessor.hpp>

#include <chrono>
#include <thread>

// Wait at worst 1s for a given condition
#define fwTestWaitMacro(cond, ...) \
    ::fwCore::TimeStamp BOOST_PP_CAT(timeStamp, __LINE__); \
    BOOST_PP_CAT(timeStamp, \
                 __LINE__).setLifePeriod( BOOST_PP_IF( BOOST_PP_IS_EMPTY( __VA_ARGS__), 10000, \
                                                       BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__) ) ); \
    BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
    while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).periodExpired()) \
    { \
        std::this_thread::sleep_for( std::chrono::milliseconds(10)); \
    }
