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

#include "SManageTest.hpp"

#include <core/service/base.hpp>

#include <data/Composite.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SManageTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SManageTest::setUp()
{
    m_manage = service::add("sight::module::data::SManage");
}

//------------------------------------------------------------------------------

void SManageTest::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_manage->stop().get());
    service::remove(m_manage);
}

//------------------------------------------------------------------------------

void SManageTest::genericAddInCompositeTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::Composite::New();
    if(alreadyPresent)
    {
        (*container)["myKey"] = object;
    }

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->get<sight::data::String>("myKey"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->get<sight::data::String>("myKey"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->get<sight::data::String>("myKey"));
    }
}

//------------------------------------------------------------------------------

void SManageTest::genericAddInVectorTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::Vector::New();
    if(alreadyPresent)
    {
        container->push_back(object);
    }

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != sight::data::String::dynamicCast((*container)[0]));
        CPPUNIT_ASSERT_EQUAL(*object, *sight::data::String::dynamicCast((*container)[0]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, sight::data::String::dynamicCast((*container)[0]));
    }

    m_manage->slot(slotName)->run();

    std::size_t expectedSize = 2 - static_cast<std::size_t>(slotName == "addOrSwap");
    CPPUNIT_ASSERT_EQUAL(expectedSize, container->size());
    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != sight::data::String::dynamicCast((*container)[1]));
        CPPUNIT_ASSERT_EQUAL(*object, *sight::data::String::dynamicCast((*container)[1]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, sight::data::String::dynamicCast((*container)[expectedSize - 1]));
    }
}

//------------------------------------------------------------------------------

void SManageTest::genericAddInSeriesSetTest(const std::string& slotName, bool alreadyPresent)
{
    auto object = sight::data::Series::New();
    object->setPatientName("You");
    auto container = sight::data::SeriesSet::New();
    if(alreadyPresent)
    {
        container->push_back(object);
    }

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != (*container)[0]);
        CPPUNIT_ASSERT(*object == *(*container)[0]);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, (*container)[0]);
    }

    m_manage->slot(slotName)->run();

    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), container->size());
        CPPUNIT_ASSERT(object != (*container)[0]);
        CPPUNIT_ASSERT(*object == *(*container)[0]);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
        CPPUNIT_ASSERT_EQUAL(object, (*container)[0]);
    }
}

//------------------------------------------------------------------------------

void SManageTest::genericAddInFieldTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::String::New();
    if(alreadyPresent)
    {
        container->setField("myField", object);
    }

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->getField<sight::data::String>("myField"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->getField<sight::data::String>("myField"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->getField<sight::data::String>("myField"));
    }
}

//------------------------------------------------------------------------------

void SManageTest::addInCompositeTest()
{
    genericAddInCompositeTest("add");
}

//------------------------------------------------------------------------------

void SManageTest::addInVectorTest()
{
    genericAddInVectorTest("add");
}

//------------------------------------------------------------------------------

void SManageTest::addInSeriesSetTest()
{
    genericAddInSeriesSetTest("add");
}

//------------------------------------------------------------------------------

void SManageTest::addInFieldTest()
{
    genericAddInFieldTest("add");
}

//------------------------------------------------------------------------------

void SManageTest::addCopyInCompositeTest()
{
    genericAddInCompositeTest("addCopy");
}

//------------------------------------------------------------------------------

void SManageTest::addCopyInVectorTest()
{
    genericAddInVectorTest("addCopy");
}

//------------------------------------------------------------------------------

void SManageTest::addCopyInSeriesSetTest()
{
    genericAddInSeriesSetTest("addCopy");
}

//------------------------------------------------------------------------------

void SManageTest::addCopyInFieldTest()
{
    genericAddInFieldTest("addCopy");
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndNotPresentInCompositeTest()
{
    genericAddInCompositeTest("addOrSwap");
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndNotPresentInVectorTest()
{
    genericAddInVectorTest("addOrSwap");
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndNotPresentInSeriesSetTest()
{
    genericAddInSeriesSetTest("addOrSwap");
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndNotPresentInFieldTest()
{
    genericAddInFieldTest("addOrSwap");
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndPresentInCompositeTest()
{
    genericAddInCompositeTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndPresentInVectorTest()
{
    genericAddInVectorTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndPresentInSeriesSetTest()
{
    genericAddInSeriesSetTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void SManageTest::addOrSwapAndPresentInFieldTest()
{
    genericAddInFieldTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void SManageTest::swapObjInCompositeTest()
{
    genericAddInCompositeTest("swapObj");
}

//------------------------------------------------------------------------------

void SManageTest::swapObjAndNotPresentInFieldTest()
{
    CPPUNIT_ASSERT_THROW(genericAddInFieldTest("swapObj"), sight::data::Exception);
}

//------------------------------------------------------------------------------

void SManageTest::swapObjAndPresentInFieldTest()
{
    genericAddInFieldTest("swapObj", true);
}

//------------------------------------------------------------------------------

void SManageTest::genericRemoveInCompositeTest(const std::string& slotName)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::Composite::New();
    (*container)["myKey"] = object;

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(container->get<sight::data::String>("myKey") != nullptr);
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->get<sight::data::String>("myKey") == nullptr);
}

//------------------------------------------------------------------------------

void SManageTest::genericRemoveInVectorTest(const std::string& slotName)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::Vector::New();
    container->push_back(object);

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void SManageTest::genericRemoveInSeriesSetTest(const std::string& slotName)
{
    auto object = sight::data::Series::New();
    object->setPatientName("You");
    auto container = sight::data::SeriesSet::New();
    container->push_back(object);

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void SManageTest::genericRemoveInFieldTest(const std::string& slotName)
{
    auto object    = sight::data::String::New("Hello world");
    auto container = sight::data::String::New();
    container->setField("myField", object);

    m_manage->setInOut(object, "object");
    m_manage->setInOut(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT_EQUAL(object, container->getField<sight::data::String>("myField"));
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->getField<sight::data::String>("myField") == nullptr);
}

//------------------------------------------------------------------------------

void SManageTest::removeInCompositeTest()
{
    genericRemoveInCompositeTest("remove");
}

//------------------------------------------------------------------------------

void SManageTest::removeInVectorTest()
{
    genericRemoveInVectorTest("remove");
}

//------------------------------------------------------------------------------

void SManageTest::removeInSeriesSetTest()
{
    genericRemoveInSeriesSetTest("remove");
}

//------------------------------------------------------------------------------

void SManageTest::removeInFieldTest()
{
    genericRemoveInFieldTest("remove");
}

//------------------------------------------------------------------------------

void SManageTest::removeIfPresentInCompositeTest()
{
    genericRemoveInCompositeTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void SManageTest::removeIfPresentInVectorTest()
{
    m_manage->start().get();
    //genericRemoveInVectorTest("removeIfPresent"); // TODO: fix crash
}

//------------------------------------------------------------------------------

void SManageTest::removeIfPresentInSeriesSetTest()
{
    genericRemoveInSeriesSetTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void SManageTest::removeIfPresentInFieldTest()
{
    genericRemoveInFieldTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void SManageTest::clearCompositeTest()
{
    genericRemoveInCompositeTest("clear");
}

//------------------------------------------------------------------------------

void SManageTest::clearVectorTest()
{
    genericRemoveInVectorTest("clear");
}

//------------------------------------------------------------------------------

void SManageTest::clearSeriesSetTest()
{
    genericRemoveInSeriesSetTest("clear");
}

//------------------------------------------------------------------------------

void SManageTest::clearFieldTest()
{
    genericRemoveInFieldTest("clear");
}

} // namespace sight::module::data::ut
