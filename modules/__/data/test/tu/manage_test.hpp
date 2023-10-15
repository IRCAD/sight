/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::data::ut
{

class manage_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(manage_test);
CPPUNIT_TEST(addInCompositeTest);
CPPUNIT_TEST(addInVectorTest);
CPPUNIT_TEST(addInSeriesSetTest);
CPPUNIT_TEST(addInFieldTest);

CPPUNIT_TEST(addCopyInCompositeTest);
CPPUNIT_TEST(addCopyInVectorTest);
CPPUNIT_TEST(addCopyInSeriesSetTest);
CPPUNIT_TEST(addCopyInFieldTest);

CPPUNIT_TEST(addOrSwapAndNotPresentInCompositeTest);
CPPUNIT_TEST(addOrSwapAndNotPresentInVectorTest);
CPPUNIT_TEST(addOrSwapAndNotPresentInSeriesSetTest);
CPPUNIT_TEST(addOrSwapAndNotPresentInFieldTest);

CPPUNIT_TEST(addOrSwapAndPresentInCompositeTest);
CPPUNIT_TEST(addOrSwapAndPresentInVectorTest);
CPPUNIT_TEST(addOrSwapAndPresentInSeriesSetTest);
CPPUNIT_TEST(addOrSwapAndPresentInFieldTest);

CPPUNIT_TEST(swapObjInCompositeTest);
CPPUNIT_TEST(swapObjAndNotPresentInFieldTest);
CPPUNIT_TEST(swapObjAndPresentInFieldTest);

CPPUNIT_TEST(removeInCompositeTest);
CPPUNIT_TEST(removeInVectorTest);
CPPUNIT_TEST(removeInSeriesSetTest);
CPPUNIT_TEST(removeInFieldTest);

CPPUNIT_TEST(removeIfPresentInCompositeTest);
CPPUNIT_TEST(removeIfPresentInVectorTest);
CPPUNIT_TEST(removeIfPresentInSeriesSetTest);
CPPUNIT_TEST(removeIfPresentInFieldTest);

CPPUNIT_TEST(clearCompositeTest);
CPPUNIT_TEST(clearVectorTest);
CPPUNIT_TEST(clearSeriesSetTest);
CPPUNIT_TEST(clearFieldTest);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void addInCompositeTest();
    void addInVectorTest();
    void addInSeriesSetTest();
    void addInFieldTest();

    void addCopyInCompositeTest();
    void addCopyInVectorTest();
    void addCopyInSeriesSetTest();
    void addCopyInFieldTest();

    void addOrSwapAndNotPresentInCompositeTest();
    void addOrSwapAndNotPresentInVectorTest();
    void addOrSwapAndNotPresentInSeriesSetTest();
    void addOrSwapAndNotPresentInFieldTest();

    void addOrSwapAndPresentInCompositeTest();
    void addOrSwapAndPresentInVectorTest();
    void addOrSwapAndPresentInSeriesSetTest();
    void addOrSwapAndPresentInFieldTest();

    void swapObjInCompositeTest();
    void swapObjAndNotPresentInFieldTest();
    void swapObjAndPresentInFieldTest();

    void removeInCompositeTest();
    void removeInVectorTest();
    void removeInSeriesSetTest();
    void removeInFieldTest();

    void removeIfPresentInCompositeTest();
    void removeIfPresentInVectorTest();
    void removeIfPresentInSeriesSetTest();
    void removeIfPresentInFieldTest();

    void clearCompositeTest();
    void clearVectorTest();
    void clearSeriesSetTest();
    void clearFieldTest();

private:

    void genericAddInCompositeTest(const std::string& slotName, bool alreadyPresent = false);
    void genericAddInVectorTest(const std::string& slotName, bool alreadyPresent    = false);
    void genericAddInSeriesSetTest(const std::string& slotName, bool alreadyPresent = false);
    void genericAddInFieldTest(const std::string& slotName, bool alreadyPresent     = false);

    void genericRemoveInCompositeTest(const std::string& slotName);
    void genericRemoveInVectorTest(const std::string& slotName);
    void genericRemoveInSeriesSetTest(const std::string& slotName);
    void genericRemoveInFieldTest(const std::string& slotName);

    service::base::sptr m_manage;
};

} // namespace sight::module::data::ut
