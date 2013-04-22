/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTest/generator/Mesh.hpp>
#include <fwDataTools/Mesh.hpp>

#include "MeshTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::MeshTest );

namespace fwDataTools
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal" , props->size() == 0 );
}

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToTriangularMesh()
{
    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleMesh(mesh1);
    ::fwDataTools::Mesh::shakePoint(mesh1);
    mesh1->adjustAllocatedMemory();

    ::fwData::TriangularMesh::NewSptr trian;
    ::fwDataTools::Mesh::toTriangularMesh(mesh1, trian);

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwDataTools::Mesh::fromTriangularMesh(trian, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
