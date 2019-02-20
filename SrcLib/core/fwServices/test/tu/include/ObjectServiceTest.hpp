/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwData/Object.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <condition_variable>
#include <mutex>

namespace fwServices
{
namespace ut
{

/**
 * @brief
 */
class ObjectServiceTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ObjectServiceTest );
CPPUNIT_TEST( swapTest );
CPPUNIT_TEST( registerTest );
CPPUNIT_TEST( registerKeyTest );
// Disable registerConnectionTest because it fails randomly on a busy computer (see #72)
//CPPUNIT_TEST( registerConnectionTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // @deprecated should be removed in 20.0
    void swapTest();
    // @deprecated should be removed in 20.0
    void registerTest();
    void registerKeyTest();
    void registerConnectionTest();

private:
    void registerService(::fwData::Object::sptr obj, const std::string& id);
    void unregisterService(::fwData::Object::sptr obj, const std::string& id);

    /// Synchronization with slots
    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::string m_ret;
    ::fwData::Object::csptr m_obj;
};

} //namespace ut
} //namespace fwServices
