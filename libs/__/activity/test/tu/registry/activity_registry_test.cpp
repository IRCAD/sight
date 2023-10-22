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

#include "activity_registry_test.hpp"

#include <activity/extension/activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/vector.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::activity_registry_test);

namespace sight::activity::ut
{

struct activity_registry_testPimpl
{
    public:

        activity::extension::activity::sptr activities;
};

//------------------------------------------------------------------------------

void activity_registry_test::setUp()
{
    m_pimpl             = std::make_shared<activity_registry_testPimpl>();
    m_pimpl->activities = std::make_shared<activity::extension::activity>();

    core::runtime::add_modules(core::runtime::get_resource_file_path("tu_exec_activity"));
    core::runtime::load_module("tu_registry");

    m_pimpl->activities->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void activity_registry_test::tearDown()
{
    m_pimpl->activities->clear_registry();

    // Clean up after the test run.
    m_pimpl.reset();
}

//------------------------------------------------------------------------------

struct ActivitiesLessThanKey
{
    //------------------------------------------------------------------------------

    inline bool operator()(
        const activity::extension::activity_info& _a,
        const activity::extension::activity_info& _b
)
    {
        return _a.id < _b.id;
    }
};

//------------------------------------------------------------------------------

void activity_registry_test::registry_test()
{
    data::vector::sptr v = std::make_shared<data::vector>();
    activity::extension::activity::infos_t activities;

    // 1 image
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(4), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry1"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(2).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(3).id);

    // 2 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry6"), activities.at(2).id);

    // 3 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 4 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 5 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 6 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 7 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 8 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 9 images
    v->push_back(std::make_shared<data::image>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 9 images, 1 mesh
    v->push_back(std::make_shared<data::mesh>());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), activities.size());

    // 1 images, 1 mesh
    v->clear();
    v->push_back(std::make_shared<data::image>());
    v->push_back(std::make_shared<data::mesh>());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry2"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(1).id);

    // 1 images, 2 mesh, 1 ImageSeries, 1 ModelSeries
    v->push_back(std::make_shared<data::mesh>());
    v->push_back(std::make_shared<data::image_series>());
    v->push_back(std::make_shared<data::model_series>());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), ActivitiesLessThanKey());

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), activities.size());
    const activity::extension::activity_info& info = activities[0];
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry0"), info.id);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), info.appConfig.parameters.size());
    CPPUNIT_ASSERT_EQUAL(std::string("orientation"), info.appConfig.parameters.at(0).replace);
    CPPUNIT_ASSERT_EQUAL(std::string("frontal"), info.appConfig.parameters.at(0).by);
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
