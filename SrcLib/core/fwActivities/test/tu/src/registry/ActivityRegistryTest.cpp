/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Vector.hpp>

#include <fwActivities/registry/Activities.hpp>

#include "registry/ActivityRegistryTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::ActivityRegistryTest );

namespace fwActivities
{
namespace ut
{

//------------------------------------------------------------------------------

void ActivityRegistryTest::setUp()
{

    ::boost::filesystem::path plugin = "share/tu_exec_fwActivities_0-0/tu_registry";
    m_bundle = ::fwRuntime::io::BundleDescriptorReader::createBundle(plugin);

    m_activities = fwActivities::registry::Activities::New();

    ::fwRuntime::Bundle::ExtensionContainer extensionsSet( m_bundle->extensionsBegin(), m_bundle->extensionsEnd());
    std::vector< SPTR( ::fwRuntime::Extension ) > extensions(extensionsSet.begin(), extensionsSet.end());
    m_activities->parseBundleInformation(extensions);

    CPPUNIT_ASSERT_EQUAL( size_t(7) , extensions.size());

    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ActivityRegistryTest::tearDown()
{
    // Clean up after the test run.
    m_bundle.reset();
}

//------------------------------------------------------------------------------

struct activities_less_than_key
{
    inline bool operator() (const ::fwActivities::registry::ActivityInfo& a,
                            const ::fwActivities::registry::ActivityInfo& b)
    {
        return (a.id < b.id);
    }
};

void ActivityRegistryTest::registryTest()
{

    ::fwData::Vector::sptr v = ::fwData::Vector::New();
    v->getContainer().push_back( ::fwData::Image::New() );

    ::fwActivities::registry::Activities::ActivitiesType activities;
    activities = m_activities->getInfos(v);

    std::sort(activities.begin(), activities.end(), activities_less_than_key());

    CPPUNIT_ASSERT_EQUAL( size_t(3), activities.size() );
    CPPUNIT_ASSERT_EQUAL( std::string("Test1"), activities.at(0).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test3"), activities.at(1).id );
    CPPUNIT_ASSERT_EQUAL( std::string("Test4"), activities.at(2).id );

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
