/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <time.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/assign/std/vector.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Array.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/Patient.hpp>
#include <fwDataTools/Image.hpp>
#include <fwDataTools/MeshGenerator.hpp>
#include <fwDataTools/ObjectGenerator.hpp>
#include <fwDataTools/ObjectComparator.hpp>

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

void ObjectTest::arrayTest()
{

    ::fwData::Array::SizeType arraySize;

    const std::string arrayType("uint8");
    arraySize.push_back(5);
    arraySize.push_back(10);

    ::fwData::Array::sptr array1 = ::fwDataTools::ObjectGenerator::randomizeArray(arrayType, arraySize);

    ::fwData::Array::sptr array2 = ::fwData::Array::dynamicCast(ObjectTest::serialize("Array.xml", array1));
    CPPUNIT_ASSERT(array2);
    CPPUNIT_ASSERT(::fwDataTools::Image::compareArray(array1, array2));
}

//------------------------------------------------------------------------------

void ObjectTest::meshTest()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::Patient::generateMesh( mesh1 );

    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::dynamicCast(ObjectTest::serialize("Mesh.xml", mesh1));
    CPPUNIT_ASSERT(mesh2);

    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareMesh( mesh1, mesh2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::imageTest()
{
    ::fwData::Image::NewSptr image1;
    ::fwDataTools::Image::generateRandomImage( image1, ::fwTools::Type::create("int16") );

    ::fwData::Image::sptr image2 = ::fwData::Image::dynamicCast( ObjectTest::serialize("Image.xml", image1) );

    CPPUNIT_ASSERT(image2);
    CPPUNIT_ASSERT( ::fwDataTools::Image::compareImage( image1, image2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::reconstructionTest()
{
    ::fwData::Reconstruction::NewSptr rec1;
    ::fwDataTools::Patient::generateReconstruction(rec1);

    ::fwData::Reconstruction::sptr rec2 = ::fwData::Reconstruction::dynamicCast( ObjectTest::serialize("Reconstruction.xml", rec1) );

    CPPUNIT_ASSERT(rec2);
    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareReconstruction( rec1, rec2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::acquisitionTest()
{
    ::fwData::Acquisition::NewSptr acq1;
    ::fwDataTools::Patient::generateAcquisition( acq1, 1 );
    ::fwData::Acquisition::sptr acq2 = ::fwData::Acquisition::dynamicCast( ObjectTest::serialize("Acquisition.xml", acq1) );
    CPPUNIT_ASSERT(acq2);
    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareAcquisition( acq1, acq2 ) );

    ::fwData::Acquisition::NewSptr acq1bis;
    ::fwDataTools::Patient::generateAcquisition( acq1bis, 0 );
    acq2 = ::fwData::Acquisition::dynamicCast( ObjectTest::serialize("Acquisition.xml", acq1bis) );
    CPPUNIT_ASSERT(acq2);
    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareAcquisition( acq1bis, acq2 ) );
}


//------------------------------------------------------------------------------

void ObjectTest::studyTest()
{
    ::fwData::Study::NewSptr study;
    ::fwDataTools::Patient::generateStudy(study, 2,2);

    ::fwData::Study::sptr study2 = ::fwData::Study::dynamicCast(ObjectTest::serialize("Study.xml", study));
    CPPUNIT_ASSERT(study2);

    CPPUNIT_ASSERT( ::fwDataTools::Patient::compareStudy( study, study2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::patientTest()
{
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient,2,2,2);

    ::fwData::Patient::sptr patient2 = ::fwData::Patient::dynamicCast(ObjectTest::serialize("Patient.xml", patient));
    CPPUNIT_ASSERT(patient2);

    CPPUNIT_ASSERT( ::fwDataTools::Patient::comparePatient( patient, patient2 ) );
}

//------------------------------------------------------------------------------

void ObjectTest::patientDBTest()
{
    ::fwData::PatientDB::NewSptr pdb1;
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient(patient,2,2,2);
    pdb1->addPatient( patient );

    ::fwData::PatientDB::sptr pdb2 = ::fwData::PatientDB::dynamicCast(ObjectTest::serialize("PatientDB.xml", pdb1));
    CPPUNIT_ASSERT(pdb2);

    CPPUNIT_ASSERT( pdb2->getNumberOfPatients() == 1 );
    CPPUNIT_ASSERT( ::fwDataTools::Patient::comparePatient( pdb1->getPatients()[0], pdb2->getPatients()[0] ) );
}

//------------------------------------------------------------------------------
void ObjectTest::colorTest()
{
    ::fwData::Color::sptr col1 = ::fwDataTools::ObjectGenerator::randomizeColor();
    ::fwData::Color::sptr col2 = ::fwData::Color::dynamicCast(ObjectTest::serialize("Color.xml", col1));
    CPPUNIT_ASSERT(col2);
    CPPUNIT_ASSERT(::fwDataTools::Patient::compareColor(col1, col2));
}

//------------------------------------------------------------------------------

void ObjectTest::materialTest()
{
    ::fwData::Material::sptr mat1 = ::fwDataTools::ObjectGenerator::createMaterial();
    ::fwData::Material::sptr mat2 = ::fwData::Material::dynamicCast(ObjectTest::serialize("Material.xml", mat1));
    CPPUNIT_ASSERT(mat2);
    CPPUNIT_ASSERT(::fwDataTools::Patient::compareMaterial(mat1, mat2));
}

//------------------------------------------------------------------------------

void ObjectTest::transferFunctionTest()
{
    ::fwData::TransfertFunction_VERSION_II::sptr tf1 = ::fwDataTools::ObjectGenerator::createTFColor();

    ::fwData::TransfertFunction_VERSION_II::sptr tf2 = ::fwData::TransfertFunction_VERSION_II::dynamicCast(ObjectTest::serialize("TransfertFunction.xml", tf1));
    CPPUNIT_ASSERT(tf2);
    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::compareTransfertFunction(tf1, tf2));
}

//------------------------------------------------------------------------------

void ObjectTest::structureTraitsDictionaryTest()
{
    ::fwData::StructureTraitsDictionary::sptr structureDico1 = ::fwDataTools::ObjectGenerator::createStructureTraitsDictionary();
    ::fwData::StructureTraitsDictionary::sptr structureDico2 = ::fwData::StructureTraitsDictionary::dynamicCast(ObjectTest::serialize("StructureTraitsDictionary.xml", structureDico1));
    CPPUNIT_ASSERT(structureDico2);
    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::compareStructureTraitsDictionary(structureDico1, structureDico2));
}

//------------------------------------------------------------------------------

void ObjectTest::structureTraitsTest()
{
    ::fwData::StructureTraits::sptr structure1 = ::fwDataTools::ObjectGenerator::createStructureTraits();
    ::fwData::StructureTraits::sptr structure2 = ::fwData::StructureTraits::dynamicCast(ObjectTest::serialize("StructureTraits.xml", structure1));
    CPPUNIT_ASSERT(structure2);
    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::compareStructureTraits(structure1, structure2));
}

//------------------------------------------------------------------------------

void ObjectTest::ROITraitsTest()
{
    ::fwData::Composite::sptr roiCompo1 = ::fwDataTools::ObjectGenerator::createROITraits();
    ::fwData::Composite::sptr roiCompo2 = ::fwData::Composite::dynamicCast(ObjectTest::serialize("ROITraits.xml", roiCompo1));
    CPPUNIT_ASSERT(roiCompo2);
    CPPUNIT_ASSERT(roiCompo2->find("ROITraits") != roiCompo2->end());
    ::fwData::ROITraits::sptr roi1 = ::fwData::ROITraits::dynamicCast((*roiCompo1)["ROITraits"]);
    ::fwData::ROITraits::sptr roi2 = ::fwData::ROITraits::dynamicCast((*roiCompo2)["ROITraits"]);
    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::compareROITraits(roi1, roi2));
}

//------------------------------------------------------------------------------

void ObjectTest::reconstructionTraitsTest()
{
    ::fwData::Composite::sptr recCompo1 = ::fwDataTools::ObjectGenerator::createReconstructionTraits();
    ::fwData::Composite::sptr recCompo2 = ::fwData::Composite::dynamicCast(ObjectTest::serialize("ReconstructionTraits.xml", recCompo1));
    CPPUNIT_ASSERT(recCompo2);
    CPPUNIT_ASSERT(recCompo2->find("ReconstructionTraits") != recCompo2->end());
    ::fwData::ReconstructionTraits::sptr rec1 = ::fwData::ReconstructionTraits::dynamicCast((*recCompo1)["ReconstructionTraits"]);
    ::fwData::ReconstructionTraits::sptr rec2 = ::fwData::ReconstructionTraits::dynamicCast((*recCompo2)["ReconstructionTraits"]);
    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::compareReconstructionTraits(rec1, rec2));
}

//------------------------------------------------------------------------------

void ObjectTest::fieldSerializationTest()
{
    ::fwData::Color::sptr mainObj = ::fwDataTools::ObjectGenerator::randomizeColor();

    ::fwData::String::NewSptr obj1 ("toto");
    std::string key1 = "key1";
    mainObj->setField_NEWAPI( key1, obj1 );

    ::fwData::Integer::NewSptr obj2 (3);
    std::string key2 = "key2";
    mainObj->setField_NEWAPI( key2, obj2 );

    ::fwData::String::NewSptr obj3 ("tutu");
    std::string key3 = "key3";
    mainObj->setField_NEWAPI( key3, obj3 );
    obj1->setField_NEWAPI( key3, obj3 );

    ::fwData::Color::sptr mainObjReloaded = ::fwData::Color::dynamicCast(ObjectTest::serialize("fieldSerialization.xml", mainObj));
    CPPUNIT_ASSERT(mainObjReloaded);

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), mainObjReloaded->getFields_NEWAPI().size() );

    CPPUNIT_ASSERT( mainObjReloaded->getField_NEWAPI( key1 ) );
    CPPUNIT_ASSERT_EQUAL( std::string("toto"), mainObjReloaded->getField_NEWAPI< ::fwData::String >( key1 )->value() );

    CPPUNIT_ASSERT( mainObjReloaded->getField_NEWAPI( key2 ) );
    CPPUNIT_ASSERT_EQUAL( 3, mainObjReloaded->getField_NEWAPI< ::fwData::Integer >( key2 )->value() );

    CPPUNIT_ASSERT( mainObjReloaded->getField_NEWAPI( key3 ) );
    CPPUNIT_ASSERT_EQUAL( std::string("tutu"), mainObjReloaded->getField_NEWAPI< ::fwData::String >(key3)->value() );

    CPPUNIT_ASSERT( mainObjReloaded->getField_NEWAPI< ::fwData::String >( key1 )->getField_NEWAPI( key3 ) );
    CPPUNIT_ASSERT_EQUAL( std::string("tutu"), mainObjReloaded->getField_NEWAPI< ::fwData::String >( key1 )->getField_NEWAPI< ::fwData::String >(key3)->value() );

    CPPUNIT_ASSERT( mainObjReloaded->getField_NEWAPI(key1)->getField_NEWAPI(key3) ==  mainObjReloaded->getField_NEWAPI(key3) );
}

//------------------------------------------------------------------------------

void ObjectTest::imageFieldSerializationTest()
{
    ::fwData::Patient::NewSptr patient;
    ::fwDataTools::Patient::generatePatient( patient, 1, 1, 0 );

    ::fwData::Image::sptr imgAcq = patient->getStudies()[0]->getAcquisitions()[0]->getImage();

    std::string key = "image";
    patient->setField_NEWAPI( key, imgAcq );

    ::fwData::Patient::sptr objReloaded = ::fwData::Patient::dynamicCast(ObjectTest::serialize("imageFieldSerializationSerialization.xml", patient));
    CPPUNIT_ASSERT( objReloaded );
    CPPUNIT_ASSERT( objReloaded->getStudies().size() == 1 );
    CPPUNIT_ASSERT( objReloaded->getStudies()[0]->getAcquisitions().size() == 1 );
    ::fwData::Image::sptr imgAcqReloaded = objReloaded->getStudies()[0]->getAcquisitions()[0]->getImage();
    CPPUNIT_ASSERT( imgAcqReloaded );
    CPPUNIT_ASSERT( objReloaded->getField_NEWAPI( key ) );
    CPPUNIT_ASSERT( objReloaded->getField_NEWAPI( key ) == imgAcqReloaded );
}

//------------------------------------------------------------------------------
