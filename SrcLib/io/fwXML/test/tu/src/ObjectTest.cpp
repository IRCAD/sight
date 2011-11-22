/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <time.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>

#include "ObjectTest.hpp"
#include "ObjectGenerator.hpp"
#include "ObjectComparator.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ObjectTest );

//------------------------------------------------------------------------------

void ObjectTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

::fwData::Object::sptr ObjectTest::serialize(std::string fileName, ::fwData::Object::sptr obj1)
{
    ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() / "ObjectTest" / fileName;
    // save Array in fwXML
    ::boost::filesystem::create_directories( testFile.parent_path() );

    ::fwXML::Serializer serializer;
    ::fwXML::NeverSplitPolicy::sptr spolicy( new ::fwXML::NeverSplitPolicy() );
    serializer.setSplitPolicy( spolicy );

#if BOOST_FILESYSTEM_VERSION > 2
    ::fwXML::UniquePathPolicy::sptr pPathPolicy ( new ::fwXML::UniquePathPolicy( testFile.filename().string() ) );
#else
    ::fwXML::UniquePathPolicy::sptr pPathPolicy ( new ::fwXML::UniquePathPolicy( testFile.leaf() ) );
#endif
    serializer.setPathPolicy( pPathPolicy );

    serializer.rootFolder() = testFile.parent_path().string();
    bool doSaveSchema = true;
    serializer.serialize(obj1, doSaveSchema);
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    // load object
    ::fwData::Object::sptr obj2 = ::fwData::Object::dynamicCast(serializer.deSerialize(testFile, true));

    // check object
    CPPUNIT_ASSERT(obj2);

    ::boost::filesystem::remove_all( testFile.parent_path().string() );

    return obj2;
}

//------------------------------------------------------------------------------

void ObjectTest::testArray()
{
    ::fwData::Array::sptr array1 = ObjectGenerator::createArray();
    ::fwData::Array::sptr array2 = ::fwData::Array::dynamicCast(ObjectTest::serialize("Array.xml", array1));
    CPPUNIT_ASSERT(array2);
    ObjectComparator::compareArray(array1, array2);
}

//------------------------------------------------------------------------------

void ObjectTest::testMesh()
{
    ::fwData::Mesh::sptr mesh1 = ObjectGenerator::createMesh();
    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::dynamicCast(ObjectTest::serialize("Mesh.xml", mesh1));
    CPPUNIT_ASSERT(mesh2);
    ObjectComparator::compareMesh(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void ObjectTest::testPatientDB()
{
    ::fwData::PatientDB::sptr pdb1 = ObjectGenerator::createPatientDB();
    ::fwData::PatientDB::sptr pdb2 = ::fwData::PatientDB::dynamicCast(ObjectTest::serialize("PatientDB.xml", pdb1));
    CPPUNIT_ASSERT(pdb2);
    ObjectComparator::comparePatientDB(pdb1, pdb2);
}

//------------------------------------------------------------------------------

void ObjectTest::testColor()
{
    ::fwData::Color::sptr col1 = ObjectGenerator::createColor();
    ::fwData::Color::sptr col2 = ::fwData::Color::dynamicCast(ObjectTest::serialize("Color.xml", col1));
    CPPUNIT_ASSERT(col2);
    ObjectComparator::compareColor(col1, col2);
}

//------------------------------------------------------------------------------

void ObjectTest::testMaterial()
{
    ::fwData::Material::sptr mat1 = ObjectGenerator::createMaterial();
    ::fwData::Material::sptr mat2 = ::fwData::Material::dynamicCast(ObjectTest::serialize("Material.xml", mat1));
    CPPUNIT_ASSERT(mat2);
    ObjectComparator::compareMaterial(mat1, mat2);
}

//------------------------------------------------------------------------------

void ObjectTest::testTriangularMesh()
{
    ::fwData::TriangularMesh::sptr trian1 = ObjectGenerator::createTriangularMesh();
    ::fwData::TriangularMesh::sptr trian2 = ::fwData::TriangularMesh::dynamicCast(ObjectTest::serialize("TriangularMesh.xml", trian1));
    CPPUNIT_ASSERT(trian2);
    ObjectComparator::compareTriangularMesh(trian1, trian2);
}

//------------------------------------------------------------------------------

void ObjectTest::testTransfertFunction()
{
    ::fwData::TransfertFunction::sptr tf1 = ObjectGenerator::createTransfertFunction();
    ::fwData::TransfertFunction::sptr tf2 = ::fwData::TransfertFunction::dynamicCast(ObjectTest::serialize("TriangularMesh.xml", tf1));
    CPPUNIT_ASSERT(tf2);
    ObjectComparator::compareTransfertFunction(tf1, tf2);
}

//------------------------------------------------------------------------------
