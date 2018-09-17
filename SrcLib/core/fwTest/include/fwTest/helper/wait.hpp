/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwCore/TimeStamp.hpp>

#include <chrono>
#include <thread>

// Wait at worst 1s for a given condition
#define fwTestWaitMacro(cond) \
    ::fwCore::TimeStamp BOOST_PP_CAT(timeStamp, __LINE__); \
    BOOST_PP_CAT(timeStamp, __LINE__).setLifePeriod(1000); \
    BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
    while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).periodExpired()) \
    { \
        std::this_thread::sleep_for( std::chrono::milliseconds(10)); \
    }
