/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

// cspell:ignore Doesnt

#include "SStarterTest.hpp"

#include <service/base.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::com::ut::SStarterTest);

namespace sight::module::ui::base::com::ut
{

class DummyService : public service::IService
{
public:

    SIGHT_DECLARE_SERVICE(DummyService, service::IService);

    int nbUpdate = 0;

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        nbUpdate++;
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }
};

SIGHT_REGISTER_SERVICE(service::IService, DummyService);

//------------------------------------------------------------------------------

void SStarterTest::setUp()
{
    m_starter = service::add("sight::module::ui::base::com::SStarter");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::base::com::SStarter'", m_starter);
}

//------------------------------------------------------------------------------

void SStarterTest::tearDown()
{
    if(m_dummyService != nullptr)
    {
        if(!m_dummyService->isStopped())
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummyService->stop().get());
        }

        service::remove(m_dummyService);
        m_dummyService = nullptr;
    }

    if(!m_starter->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_starter->stop().get());
    }

    service::remove(m_starter);
}

//------------------------------------------------------------------------------

void SStarterTest::test(const std::string& actionName, bool exists, ShouldBe shouldBe, int nbUpdate)
{
    if(exists)
    {
        service::IService::sptr targetService = service::add("DummyService", "targetService");
        CPPUNIT_ASSERT_MESSAGE("Failed to create service 'DummyService'", targetService);
        m_dummyService = std::dynamic_pointer_cast<DummyService>(targetService);
        CPPUNIT_ASSERT_MESSAGE("Failed to cast service to 'DummyService'", m_dummyService);
        CPPUNIT_ASSERT_NO_THROW(m_dummyService->configure());
        if(shouldBe == ShouldBe::STOPPED)
        {
            CPPUNIT_ASSERT_NO_THROW(m_dummyService->start().get());
        }
    }

    boost::property_tree::ptree ptree;
    ptree.put(actionName + ".<xmlattr>.uid", "targetService");
    m_starter->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_starter->configure());
    CPPUNIT_ASSERT_NO_THROW(m_starter->start().get());
    CPPUNIT_ASSERT_NO_THROW(m_starter->update().get());

    if(exists)
    {
        if(shouldBe == ShouldBe::STARTED)
        {
            CPPUNIT_ASSERT(m_dummyService->isStarted());
        }
        else if(shouldBe == ShouldBe::STOPPED)
        {
            CPPUNIT_ASSERT(m_dummyService->isStopped());
        }
        else
        {
            CPPUNIT_ASSERT(m_dummyService->isStarted());
            CPPUNIT_ASSERT_EQUAL(nbUpdate, m_dummyService->nbUpdate);
            CPPUNIT_ASSERT_NO_THROW(m_starter->update().get());
            CPPUNIT_ASSERT(m_dummyService->isStopped());
        }

        CPPUNIT_ASSERT_EQUAL(nbUpdate, m_dummyService->nbUpdate);
    }
}

//------------------------------------------------------------------------------

void SStarterTest::startTest()
{
    test("start", true, ShouldBe::STARTED, 1);
}

//------------------------------------------------------------------------------

void SStarterTest::startOnlyTest()
{
    test("start_only", true, ShouldBe::STARTED, 0);
}

//------------------------------------------------------------------------------

void SStarterTest::startIfExistsTest()
{
    test("start_if_exists", true, ShouldBe::STARTED, 1);
}

//------------------------------------------------------------------------------

void SStarterTest::startIfExistsButDoesntExistTest()
{
    test("start_if_exists", false);
}

//------------------------------------------------------------------------------

void SStarterTest::stopTest()
{
    test("stop", true, ShouldBe::STOPPED);
}

//------------------------------------------------------------------------------

void SStarterTest::stopIfExistsTest()
{
    test("stop_if_exists", true, ShouldBe::STOPPED);
}

//------------------------------------------------------------------------------

void SStarterTest::stopIfExistsButDoesntExistTest()
{
    test("stop_if_exists", false);
}

//------------------------------------------------------------------------------

void SStarterTest::startOrStopTest()
{
    test("start_or_stop", true, ShouldBe::STARTED_THEN_STOPPED, 1);
}

//------------------------------------------------------------------------------

void SStarterTest::startOnlyOrStopTest()
{
    test("start_only_or_stop", true, ShouldBe::STARTED_THEN_STOPPED, 0);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::com::ut
