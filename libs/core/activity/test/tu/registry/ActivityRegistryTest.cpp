/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Vector.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::activity::ut::ActivityRegistryTest);

namespace sight::activity
{

namespace ut
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
    m_pimpl->activities = activity::extension::Activity::New();

    core::runtime::addModules(core::runtime::getResourceFilePath("tu_exec_activity"));
    core::runtime::loadModule("tu_registry");

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

struct activities_less_than_key
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
    data::Vector::sptr v = data::Vector::New();
    activity::extension::Activity::ActivitiesType activities;

    // 1 image
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(4), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry1"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(2).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(3).id);

    // 2 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(3), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry3"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(1).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry6"), activities.at(2).id);

    // 3 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 4 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 5 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 6 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 7 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 8 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry5"), activities.at(1).id);

    // 9 images
    v->getContainer().push_back(data::Image::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(1), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry4"), activities.at(0).id);

    // 9 images, 1 mesh
    v->getContainer().push_back(data::Mesh::New());
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(0), activities.size());

    // 1 images, 1 mesh
    v->getContainer().clear();
    v->getContainer().push_back(data::Image::New());
    v->getContainer().push_back(data::Mesh::New());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(2), activities.size());
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry2"), activities.at(0).id);
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry7"), activities.at(1).id);

    // 1 images, 2 mesh, 1 ImageSeries, 1 ModelSeries
    v->getContainer().push_back(data::Mesh::New());
    v->getContainer().push_back(data::ImageSeries::New());
    v->getContainer().push_back(data::ModelSeries::New());

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL(size_t(1), activities.size());
    const activity::extension::ActivityInfo& info = activities[0];
    CPPUNIT_ASSERT_EQUAL(std::string("TestRegistry0"), info.id);
    CPPUNIT_ASSERT_EQUAL(size_t(3), info.appConfig.parameters.size());
    CPPUNIT_ASSERT_EQUAL(std::string("refImageUid"), info.appConfig.parameters.at(0).replace);
    CPPUNIT_ASSERT_EQUAL(std::string("@values.param1"), info.appConfig.parameters.at(0).by);
    CPPUNIT_ASSERT_EQUAL(std::string("registeredImageUid"), info.appConfig.parameters.at(1).replace);
    CPPUNIT_ASSERT_EQUAL(std::string("@values.param2"), info.appConfig.parameters.at(1).by);
    CPPUNIT_ASSERT_EQUAL(std::string("orientation"), info.appConfig.parameters.at(2).replace);
    CPPUNIT_ASSERT_EQUAL(std::string("frontal"), info.appConfig.parameters.at(2).by);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::activity
