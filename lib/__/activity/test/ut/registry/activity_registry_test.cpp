/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <activity/extension/activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/vector.hpp>

#include <doctest/doctest.h>

#include <algorithm>

namespace
{

struct activity_registry_test_pimpl
{
    public:

        sight::activity::extension::activity::sptr activities;
};

//------------------------------------------------------------------------------

struct activity_registry_fixture
{
    std::shared_ptr<activity_registry_test_pimpl> m_pimpl;

    activity_registry_fixture()
    {
        m_pimpl             = std::make_shared<activity_registry_test_pimpl>();
        m_pimpl->activities = std::make_shared<sight::activity::extension::activity>();

        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("activity_ut"));
        sight::core::runtime::load_module("ut_registry");

        m_pimpl->activities->parse_plugin_infos();
    }

    ~activity_registry_fixture()
    {
        m_pimpl->activities->clear_registry();

        // Clean up after the test run.
        m_pimpl.reset();
    }
};

struct activities_less_than_key
{
    //------------------------------------------------------------------------------

    bool operator()(
        const sight::activity::extension::activity_info& _a,
        const sight::activity::extension::activity_info& _b
)
    {
        return _a.id < _b.id;
    }
};

} // namespace

TEST_SUITE("sight::activity::activity_registry")
{
    TEST_CASE_FIXTURE(activity_registry_fixture, "registry_test")
    {
        sight::data::vector::sptr v = std::make_shared<sight::data::vector>();
        sight::activity::extension::activity::infos_t activities;

        // 1 image
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(4), activities.size());
        CHECK_EQ(std::string("TestRegistry1"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry3"), activities.at(1).id);
        CHECK_EQ(std::string("TestRegistry4"), activities.at(2).id);
        CHECK_EQ(std::string("TestRegistry7"), activities.at(3).id);

        // 2 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(3), activities.size());
        CHECK_EQ(std::string("TestRegistry3"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry4"), activities.at(1).id);
        CHECK_EQ(std::string("TestRegistry6"), activities.at(2).id);

        // 3 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(1), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);

        // 4 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry5"), activities.at(1).id);

        // 5 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry5"), activities.at(1).id);

        // 6 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry5"), activities.at(1).id);

        // 7 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry5"), activities.at(1).id);

        // 8 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry5"), activities.at(1).id);

        // 9 images
        v->push_back(std::make_shared<sight::data::image>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(1), activities.size());
        CHECK_EQ(std::string("TestRegistry4"), activities.at(0).id);

        // 9 images, 1 mesh
        v->push_back(std::make_shared<sight::data::mesh>());
        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(0), activities.size());

        // 1 images, 1 mesh
        v->clear();
        v->push_back(std::make_shared<sight::data::image>());
        v->push_back(std::make_shared<sight::data::mesh>());

        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(2), activities.size());
        CHECK_EQ(std::string("TestRegistry2"), activities.at(0).id);
        CHECK_EQ(std::string("TestRegistry7"), activities.at(1).id);

        // 1 images, 2 mesh, 1 ImageSeries, 1 ModelSeries
        v->push_back(std::make_shared<sight::data::mesh>());
        v->push_back(std::make_shared<sight::data::image_series>());
        v->push_back(std::make_shared<sight::data::model_series>());

        activities = m_pimpl->activities->get_infos(v);
        std::ranges::sort(activities, activities_less_than_key());

        CHECK_EQ(std::size_t(1), activities.size());
        const sight::activity::extension::activity_info& info = activities[0];
        CHECK_EQ(std::string("TestRegistry0"), info.id);
        CHECK_EQ(std::size_t(1), info.app_config.parameters.size());
        CHECK_EQ(std::string("orientation"), info.app_config.parameters.at(0).replace);
        CHECK_EQ(std::string("frontal"), info.app_config.parameters.at(0).by);
    }

//------------------------------------------------------------------------------
}
