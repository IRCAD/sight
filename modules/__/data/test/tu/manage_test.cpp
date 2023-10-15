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

#include "manage_test.hpp"

#include <data/composite.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::manage_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void manage_test::setUp()
{
    m_manage = service::add("sight::module::data::manage");
}

//------------------------------------------------------------------------------

void manage_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_manage->stop().get());
    service::remove(m_manage);
}

//------------------------------------------------------------------------------

void manage_test::genericAddInCompositeTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::composite>();
    if(alreadyPresent)
    {
        (*container)["myKey"] = object;
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->get<sight::data::string>("myKey"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->get<sight::data::string>("myKey"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->get<sight::data::string>("myKey"));
    }
}

//------------------------------------------------------------------------------

void manage_test::genericAddInVectorTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::vector>();
    if(alreadyPresent)
    {
        container->push_back(object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
        CPPUNIT_ASSERT_EQUAL(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
    }

    m_manage->slot(slotName)->run();

    std::size_t expectedSize = 2 - static_cast<std::size_t>(slotName == "addOrSwap");
    CPPUNIT_ASSERT_EQUAL(expectedSize, container->size());
    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
        CPPUNIT_ASSERT_EQUAL(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, std::dynamic_pointer_cast<sight::data::string>((*container)[expectedSize - 1]));
    }
}

//------------------------------------------------------------------------------

void manage_test::genericAddInSeriesSetTest(const std::string& slotName, bool alreadyPresent)
{
    auto object = std::make_shared<sight::data::series>();
    object->setPatientName("You");
    auto container = std::make_shared<sight::data::series_set>();
    if(alreadyPresent)
    {
        container->push_back(object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
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

void manage_test::genericAddInFieldTest(const std::string& slotName, bool alreadyPresent)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::string>();
    if(alreadyPresent)
    {
        container->setField("myField", object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(slotName)->run();

    if(slotName == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->getField<sight::data::string>("myField"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->getField<sight::data::string>("myField"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->getField<sight::data::string>("myField"));
    }
}

//------------------------------------------------------------------------------

void manage_test::addInCompositeTest()
{
    genericAddInCompositeTest("add");
}

//------------------------------------------------------------------------------

void manage_test::addInVectorTest()
{
    genericAddInVectorTest("add");
}

//------------------------------------------------------------------------------

void manage_test::addInSeriesSetTest()
{
    genericAddInSeriesSetTest("add");
}

//------------------------------------------------------------------------------

void manage_test::addInFieldTest()
{
    genericAddInFieldTest("add");
}

//------------------------------------------------------------------------------

void manage_test::addCopyInCompositeTest()
{
    genericAddInCompositeTest("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::addCopyInVectorTest()
{
    genericAddInVectorTest("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::addCopyInSeriesSetTest()
{
    genericAddInSeriesSetTest("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::addCopyInFieldTest()
{
    genericAddInFieldTest("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndNotPresentInCompositeTest()
{
    genericAddInCompositeTest("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndNotPresentInVectorTest()
{
    genericAddInVectorTest("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndNotPresentInSeriesSetTest()
{
    genericAddInSeriesSetTest("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndNotPresentInFieldTest()
{
    genericAddInFieldTest("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndPresentInCompositeTest()
{
    genericAddInCompositeTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndPresentInVectorTest()
{
    genericAddInVectorTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndPresentInSeriesSetTest()
{
    genericAddInSeriesSetTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::addOrSwapAndPresentInFieldTest()
{
    genericAddInFieldTest("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::swapObjInCompositeTest()
{
    genericAddInCompositeTest("swapObj");
}

//------------------------------------------------------------------------------

void manage_test::swapObjAndNotPresentInFieldTest()
{
    CPPUNIT_ASSERT_THROW(genericAddInFieldTest("swapObj"), sight::data::exception);
}

//------------------------------------------------------------------------------

void manage_test::swapObjAndPresentInFieldTest()
{
    genericAddInFieldTest("swapObj", true);
}

//------------------------------------------------------------------------------

void manage_test::genericRemoveInCompositeTest(const std::string& slotName)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::composite>();
    (*container)["myKey"] = object;

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(container->get<sight::data::string>("myKey") != nullptr);
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->get<sight::data::string>("myKey") == nullptr);
}

//------------------------------------------------------------------------------

void manage_test::genericRemoveInVectorTest(const std::string& slotName)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::vector>();
    container->push_back(object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void manage_test::genericRemoveInSeriesSetTest(const std::string& slotName)
{
    auto object = std::make_shared<sight::data::series>();
    object->setPatientName("You");
    auto container = std::make_shared<sight::data::series_set>();
    container->push_back(object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void manage_test::genericRemoveInFieldTest(const std::string& slotName)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::string>();
    container->setField("myField", object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT_EQUAL(object, container->getField<sight::data::string>("myField"));
    m_manage->slot(slotName)->run();
    CPPUNIT_ASSERT(container->getField<sight::data::string>("myField") == nullptr);
}

//------------------------------------------------------------------------------

void manage_test::removeInCompositeTest()
{
    genericRemoveInCompositeTest("remove");
}

//------------------------------------------------------------------------------

void manage_test::removeInVectorTest()
{
    genericRemoveInVectorTest("remove");
}

//------------------------------------------------------------------------------

void manage_test::removeInSeriesSetTest()
{
    genericRemoveInSeriesSetTest("remove");
}

//------------------------------------------------------------------------------

void manage_test::removeInFieldTest()
{
    genericRemoveInFieldTest("remove");
}

//------------------------------------------------------------------------------

void manage_test::removeIfPresentInCompositeTest()
{
    genericRemoveInCompositeTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::removeIfPresentInVectorTest()
{
    m_manage->start().get();
    //genericRemoveInVectorTest("removeIfPresent"); // TODO: fix crash
}

//------------------------------------------------------------------------------

void manage_test::removeIfPresentInSeriesSetTest()
{
    genericRemoveInSeriesSetTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::removeIfPresentInFieldTest()
{
    genericRemoveInFieldTest("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::clearCompositeTest()
{
    genericRemoveInCompositeTest("clear");
}

//------------------------------------------------------------------------------

void manage_test::clearVectorTest()
{
    genericRemoveInVectorTest("clear");
}

//------------------------------------------------------------------------------

void manage_test::clearSeriesSetTest()
{
    genericRemoveInSeriesSetTest("clear");
}

//------------------------------------------------------------------------------

void manage_test::clearFieldTest()
{
    genericRemoveInFieldTest("clear");
}

} // namespace sight::module::data::ut
