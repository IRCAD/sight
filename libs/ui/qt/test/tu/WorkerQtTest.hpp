/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/base.hpp>
#include <core/thread/Worker.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::ui::qt
{

namespace ut
{

class WorkerQtTest : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(WorkerQtTest);

    CPPUNIT_TEST(initTest);
    CPPUNIT_TEST(twiceInitTest);
    CPPUNIT_TEST(basicTest);
    CPPUNIT_TEST(postFromInsideTest);
    CPPUNIT_TEST(postFromOutsideTest);

    CPPUNIT_TEST(basicTimerTest);

    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void initTest();
    void twiceInitTest();
    void basicTest();

    void postFromInsideTest();
    void postFromOutsideTest();

    void basicTimerTest();

protected:

    SPTR(core::thread::Worker) m_worker;
};

} //namespace ut

} //namespace sight::ui::qt
