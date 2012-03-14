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

#include <fwData/PatientDB.hpp>

#include <fwDataTools/Patient.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>


#include "ObjectTest.hpp"


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
//    ::fwData::Array::sptr array1 = ObjectGenerator::createArray();
//    ::fwData::Array::sptr array2 = ::fwData::Array::dynamicCast(ObjectTest::serialize("Array.xml", array1));
//    CPPUNIT_ASSERT(array2);
//    ObjectComparator::compareArray(array1, array2);
}

//------------------------------------------------------------------------------

void ObjectTest::testMesh()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::Patient::generateMesh( mesh1 );

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::dynamicCast(ObjectTest::serialize("Mesh.xml", mesh1));
    CPPUNIT_ASSERT(mesh2);

    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareMesh( mesh1, mesh2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::testPatientDB()
{
    ::fwData::PatientDB::NewSptr pdb1;
    ::fwData::Patient::NewSptr patient;
    pdb1->addPatient( patient );
    ::fwDataTools::Patient::generatePatient(patient,5,5,5);

    ::fwData::PatientDB::sptr pdb2 = ::fwData::PatientDB::dynamicCast(ObjectTest::serialize("PatientDB.xml", pdb1));
    CPPUNIT_ASSERT(pdb2);

    CPPUNIT_ASSERT( pdb2->getNumberOfPatients() == 1 );
    CPPUNIT_ASSERT( ::fwDataTools::Patient::comparePatient( pdb1->getPatients()[0], pdb2->getPatients()[0] ) );
}

//------------------------------------------------------------------------------

void ObjectTest::testColor()
{
//    ::fwData::Color::sptr col1 = ObjectGenerator::createColor();
//    ::fwData::Color::sptr col2 = ::fwData::Color::dynamicCast(ObjectTest::serialize("Color.xml", col1));
//    CPPUNIT_ASSERT(col2);
//    ObjectComparator::compareColor(col1, col2);
}

//------------------------------------------------------------------------------

void ObjectTest::testMaterial()
{
//    ::fwData::Material::sptr mat1 = ObjectGenerator::createMaterial();
//    ::fwData::Material::sptr mat2 = ::fwData::Material::dynamicCast(ObjectTest::serialize("Material.xml", mat1));
//    CPPUNIT_ASSERT(mat2);
//    ObjectComparator::compareMaterial(mat1, mat2);
}

//------------------------------------------------------------------------------

void ObjectTest::testTriangularMesh()
{
//    ::fwData::TriangularMesh::sptr trian1 = ObjectGenerator::createTriangularMesh();
//    ::fwData::TriangularMesh::sptr trian2 = ::fwData::TriangularMesh::dynamicCast(ObjectTest::serialize("TriangularMesh.xml", trian1));
//    CPPUNIT_ASSERT(trian2);
//    ObjectComparator::compareTriangularMesh(trian1, trian2);
}

//------------------------------------------------------------------------------

void ObjectTest::testTransfertFunction()
{
//    ::fwData::TransfertFunction::sptr tf1 = ObjectGenerator::createTransfertFunction();
//    ::fwData::TransfertFunction::sptr tf2 = ::fwData::TransfertFunction::dynamicCast(ObjectTest::serialize("TransfertFunction.xml", tf1));
//    CPPUNIT_ASSERT(tf2);
//    ObjectComparator::compareTransfertFunction(tf1, tf2);
}

//------------------------------------------------------------------------------

void ObjectTest::testStructureTraitsDictionary()
{
//    ::fwData::StructureTraitsDictionary::sptr structureDico1 = ObjectGenerator::createStructureTraitsDictionary();
//    ::fwData::StructureTraitsDictionary::sptr structureDico2 = ::fwData::StructureTraitsDictionary::dynamicCast(ObjectTest::serialize("StructureTraitsDictionary.xml", structureDico1));
//    CPPUNIT_ASSERT(structureDico2);
//    ObjectComparator::compareStructureTraitsDictionary(structureDico1, structureDico2);
}

//------------------------------------------------------------------------------

void ObjectTest::testStructureTraits()
{
//    ::fwData::StructureTraits::sptr structure1 = ObjectGenerator::createStructureTraits();
//    ::fwData::StructureTraits::sptr structure2 = ::fwData::StructureTraits::dynamicCast(ObjectTest::serialize("StructureTraits.xml", structure1));
//    CPPUNIT_ASSERT(structure2);
//    ObjectComparator::compareStructureTraits(structure1, structure2);
}

//------------------------------------------------------------------------------

void ObjectTest::testROITraits()
{
//    ::fwData::Composite::sptr roiCompo1 = ObjectGenerator::createROITraits();
//    ::fwData::Composite::sptr roiCompo2 = ::fwData::Composite::dynamicCast(ObjectTest::serialize("ROITraits.xml", roiCompo1));
//    CPPUNIT_ASSERT(roiCompo2);
//    CPPUNIT_ASSERT(roiCompo2->find("ROITraits") != roiCompo2->end());
//    ::fwData::ROITraits::sptr roi1 = ::fwData::ROITraits::dynamicCast((*roiCompo1)["ROITraits"]);
//    ::fwData::ROITraits::sptr roi2 = ::fwData::ROITraits::dynamicCast((*roiCompo2)["ROITraits"]);
//    ObjectComparator::compareROITraits(roi1, roi2);
}

//------------------------------------------------------------------------------

void ObjectTest::testReconstructionTraits()
{
//    ::fwData::Composite::sptr recCompo1 = ObjectGenerator::createReconstructionTraits();
//    ::fwData::Composite::sptr recCompo2 = ::fwData::Composite::dynamicCast(ObjectTest::serialize("ReconstructionTraits.xml", recCompo1));
//    CPPUNIT_ASSERT(recCompo2);
//    CPPUNIT_ASSERT(recCompo2->find("ReconstructionTraits") != recCompo2->end());
//    ::fwData::ReconstructionTraits::sptr rec1 = ::fwData::ReconstructionTraits::dynamicCast((*recCompo1)["ReconstructionTraits"]);
//    ::fwData::ReconstructionTraits::sptr rec2 = ::fwData::ReconstructionTraits::dynamicCast((*recCompo2)["ReconstructionTraits"]);
//    ObjectComparator::compareReconstructionTraits(rec1, rec2);
}

//------------------------------------------------------------------------------
