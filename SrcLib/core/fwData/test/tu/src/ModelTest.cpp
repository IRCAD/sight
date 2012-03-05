/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Model.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Material.hpp>
#include "ModelTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ModelTest );

namespace fwData
{
namespace ut
{

void ModelTest::setUp()
{
    // Set up context before running a test.

}
void ModelTest::tearDown()
{
    // Clean up after the test run.
}

void ModelTest::methode1()
{
    //-----------test values
    ::fwData::TriangularMesh::sptr tri = ::fwData::TriangularMesh::New();
    ::fwData::Material::sptr mat = ::fwData::Material::New();

    ::fwData::Model::NewSptr model;

    model->getRefMap()[tri] = mat;

    CPPUNIT_ASSERT(model->getRefMap().find(tri) != model->getRefMap().end());
    CPPUNIT_ASSERT_EQUAL(model->getRefMap()[tri], mat);
}

} //namespace ut
} //namespace fwData
