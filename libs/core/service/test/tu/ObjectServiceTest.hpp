/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/Object.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <condition_variable>
#include <mutex>

namespace sight::service::ut
{

/**
 * @brief
 */
class ObjectServiceTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ObjectServiceTest);
CPPUNIT_TEST(registerKeyTest);
// Disable registerConnectionTest because it fails randomly on a busy computer (see #72)
//CPPUNIT_TEST( registerConnectionTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;
    static void registerKeyTest();
    void registerConnectionTest();

private:

    void registerService(data::Object::sptr obj, const std::string& id);
    void unregisterService(data::Object::sptr obj, const std::string& id);

    /// Synchronization with slots
    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::string m_ret;
    data::Object::csptr m_obj;
};

} // namespace sight::service::ut
