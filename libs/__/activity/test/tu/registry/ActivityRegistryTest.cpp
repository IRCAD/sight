/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ActivityRegistryTest.hpp"

#include <activity/extension/Activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Vector.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::ActivityRegistryTest);

namespace sight::activity::ut
{

struct ActivityRegistryTestPimpl
{
    public:

        activity::extension::Activity::sptr activities;
};

//------------------------------------------------------------------------------

void ActivityRegistryTest::setUp()
{
    m_pimpl             = std::make_shared<ActivityRegistryTestPimpl>();
    m_pimpl->activities = std::make_shared<activity::extension::Activity>();

    core::runtime::add_modules(core::runtime::get_resource_file_path("tu_exec_activity"));
    core::runtime::load_module("tu_registry");

    m_pimpl->activities->parseBundleInformation();
}

//------------------------------------------------------------------------------

void ActivityRegistryTest::tearDown()
{
    m_pimpl->activities->clearRegistry();

    // Clean up after the test run.
    m_pimpl.reset();
}

//------------------------------------------------------------------------------

struct ActivitiesLessThanKey
{
    //------------------------------------------------------------------------------

    inline bool operator()(
        const activity::extension::ActivityInfo& a,
        const activity::extension::ActivityInfo& b
)
    {
        return a.id < b.id;
    }
};

//------------------------------------------------------------------------------

void ActivityRegistryTest::registryTest()
{
    data::Vector::sptr v = std::make_shared<data::Vector>();
    activity::extension::Activity::ActivitiesType activities;

    // 1 image
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(4), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry1"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(2).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(3).id);

    // 2 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry6"), activities.at(2).id);

    // 3 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 4 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 5 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 6 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 7 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 8 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 9 images
    v->push_back(std::make_shared<data::Image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 9 images, 1 mesh
    v->push_back(std::make_shared<data::Mesh>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), activities.size());

    // 1 images, 1 mesh
    v->clear();
    v->push_back(std::make_shared<data::Image>());
    v->push_back(std::make_shared<data::Mesh>());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry2"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(1).id);

    // 1 images, 2 mesh, 1 ImageSeries, 1 ModelSeries
    v->push_back(std::make_shared<data::Mesh>());
    v->push_back(std::make_shared<data::ImageSeries>());
    v->push_back(std::make_shared<data::ModelSeries>());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    const activity::extension::ActivityInfo& info = activities[0];
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry0"), info.id);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), info.appConfig.parameters.size());
    CPPUNIT_ASSERT_EQUAL(std::string("orientation"), info.appConfig.parameters.at(0).replace);
    CPPUNIT_ASSERT_EQUAL(std::string("frontal"), info.appConfig.parameters.at(0).by);
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
