/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "registry/ActivityRegistryTest.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/operations.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::ActivityRegistryTest );

namespace fwActivities
{
namespace ut
{

struct ActivityRegistryTestPimpl
{
    public:
        SPTR(::fwRuntime::Bundle) bundle;
        ::fwActivities::registry::Activities::sptr activities;
};

//------------------------------------------------------------------------------

void ActivityRegistryTest::setUp()
{
    m_pimpl = std::make_shared< ActivityRegistryTestPimpl >();

    std::filesystem::path plugin = ::fwRuntime::getResourceFilePath("tu_exec_fwActivities-0.0/tu_registry");
    m_pimpl->bundle = ::fwRuntime::io::BundleDescriptorReader::createBundle(plugin);

    m_pimpl->activities = fwActivities::registry::Activities::New();

    ::fwRuntime::Bundle::ExtensionContainer extensionsSet(
        m_pimpl->bundle->extensionsBegin(), m_pimpl->bundle->extensionsEnd());
    std::vector< SPTR( ::fwRuntime::Extension ) > extensions(extensionsSet.begin(), extensionsSet.end());
    m_pimpl->activities->parseBundleInformation(extensions);

    CPPUNIT_ASSERT_EQUAL( size_t(8), extensions.size());

    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ActivityRegistryTest::tearDown()
{
    // Clean up after the test run.
    m_pimpl.reset();
}

//------------------------------------------------------------------------------

struct activities_less_than_key
{
    //------------------------------------------------------------------------------

    inline bool operator() (const ::fwActivities::registry::ActivityInfo& a,
                            const ::fwActivities::registry::ActivityInfo& b)
    {
        return (a.id < b.id);
    }
};

//------------------------------------------------------------------------------

void ActivityRegistryTest::registryTest()
{

    ::fwData::Vector::sptr v = ::fwData::Vector::New();
    ::fwActivities::registry::Activities::ActivitiesType activities;

    // 1 image
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(4), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test1"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test3"), activities.at(1).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(2).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test7"), activities.at(3).id );

    // 2 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(3), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test3"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(1).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test6"), activities.at(2).id );

    // 3 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(1), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );

    // 4 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test5"), activities.at(1).id );

    // 5 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test5"), activities.at(1).id );

    // 6 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test5"), activities.at(1).id );

    // 7 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test5"), activities.at(1).id );

    // 8 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test5"), activities.at(1).id );

    // 9 images
    v->getContainer().push_back( ::fwData::Image::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(1), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(0).id );

    // 9 images, 1 mesh
    v->getContainer().push_back( ::fwData::Mesh::New() );
    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(0), activities.size() );

    // 1 images, 1 mesh
    v->getContainer().clear();
    v->getContainer().push_back( ::fwData::Image::New() );
    v->getContainer().push_back( ::fwData::Mesh::New() );

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(2), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test2"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test7"), activities.at(1).id );

    // 1 images, 2 mesh, 1 ImageSeries, 1 ModelSeries
    v->getContainer().push_back( ::fwData::Mesh::New() );
    v->getContainer().push_back( ::fwMedData::ImageSeries::New() );
    v->getContainer().push_back( ::fwMedData::ModelSeries::New() );

    activities = m_pimpl->activities->getInfos(v);
    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(1), activities.size() );
    const ::fwActivities::registry::ActivityInfo& info = activities[0];
    CPPUNIT_ASSERT_EQUAL( std::string("Test0"), info.id );
    CPPUNIT_ASSERT_EQUAL( size_t(3), info.appConfig.parameters.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("refImageUid"), info.appConfig.parameters.at(0).replace );
    CPPUNIT_ASSERT_EQUAL( std::string("@values.param1"), info.appConfig.parameters.at(0).by );
    CPPUNIT_ASSERT_EQUAL( std::string("registeredImageUid"), info.appConfig.parameters.at(1).replace );
    CPPUNIT_ASSERT_EQUAL( std::string("@values.param2"), info.appConfig.parameters.at(1).by );
    CPPUNIT_ASSERT_EQUAL( std::string("orientation"), info.appConfig.parameters.at(2).replace );
    CPPUNIT_ASSERT_EQUAL( std::string("frontal"), info.appConfig.parameters.at(2).by );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
