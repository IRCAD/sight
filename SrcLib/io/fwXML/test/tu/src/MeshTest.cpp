/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwTools/System.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>

#include "MeshTest.hpp"
#include "ArrayTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MeshTest );

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

void MeshTest::testExportImportSyntheticMesh()
{
    ::fwData::Mesh::NewSptr mesh1;
    ::fwDataTools::MeshGenerator::generateTriangleQuadMesh(mesh1);
    ::fwDataTools::MeshGenerator::shakePoint(mesh1);
    ::fwDataTools::MeshGenerator::colorizeMeshPoints(mesh1);
    ::fwDataTools::MeshGenerator::colorizeMeshCells(mesh1);
    ::fwDataTools::MeshGenerator::generatePointNormals(mesh1);
    ::fwDataTools::MeshGenerator::generateCellNormals(mesh1);

    ::fwData::Array::sptr dataArray1 = ArrayTest::generateArray();
    ::fwData::Array::sptr dataArray2 = ArrayTest::generateArray();

    mesh1->addDataArray("dataArray1", dataArray1);
    mesh1->addDataArray("dataArray2", dataArray2);

    mesh1->adjustAllocatedMemory();

    ::boost::filesystem::path testFile = ::fwTools::System::getTemporaryFolder() / "MeshTest" / "Mesh.xml";
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
    bool doSaveSchema = false;
    serializer.serialize(mesh1, doSaveSchema);
    CPPUNIT_ASSERT(::boost::filesystem::exists(testFile));

    // load Mesh
    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::dynamicCast(serializer.deSerialize(testFile, true));

    // check Mesh
    CPPUNIT_ASSERT(mesh2);
    CPPUNIT_ASSERT(mesh2->getDataArray("dataArray1"));
    CPPUNIT_ASSERT(mesh2->getDataArray("dataArray2"));

    this->compareMesh(mesh1, mesh2);

    ::boost::filesystem::remove_all( testFile.parent_path().string() );
}

//------------------------------------------------------------------------------

void MeshTest::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells() , mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize()  , mesh2->getCellDataSize());

    this->compareBuffer(mesh1->getPointsArray(), mesh2->getPointsArray());
    this->compareBuffer(mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    this->compareBuffer(mesh1->getCellDataOffsetsArray(), mesh2->getCellDataOffsetsArray());
    this->compareBuffer(mesh1->getCellDataArray(), mesh2->getCellDataArray());
    this->compareBuffer(mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    this->compareBuffer(mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    this->compareBuffer(mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    this->compareBuffer(mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());

    this->compareDataArrayMesh(mesh1, mesh2);
}
//------------------------------------------------------------------------------

void MeshTest::compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT( mesh1->getDataArrayNames() == mesh2->getDataArrayNames());

    std::vector<std::string> vectNames = mesh1->getDataArrayNames();
    BOOST_FOREACH(std::string name, vectNames)
    {
        ::fwData::Array::sptr array1 = mesh1->getDataArray(name);
        ::fwData::Array::sptr array2 = mesh2->getDataArray(name);
        this->compareBuffer(array1, array2);
    }
}

//------------------------------------------------------------------------------

void MeshTest::compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2)
{
    CPPUNIT_ASSERT( (!buff1 && !buff2) || (buff1 && buff2));
    if(buff1)
    {
        CPPUNIT_ASSERT(buff1->getSize() == buff2->getSize());

        char *iter1 = buff1->begin<char>();
        char *iter2 = buff2->begin<char>();

        for (; iter1 != buff1->end<char>() ; ++iter1, ++iter2)
        {
            CPPUNIT_ASSERT_EQUAL(*iter1, *iter2);
        }
    }
}

//------------------------------------------------------------------------------
