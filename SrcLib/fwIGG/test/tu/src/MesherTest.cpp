/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MesherTest.hpp"

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwIGG/Mesher.hpp>
#include <fwIGG/Mesher.hxx>

#include <fwItkIO/ImageReader.hpp>

#include <fwMemory/tools/MemoryMonitorTools.hpp>

#include <fwTest/Data.hpp>

#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwIGG::ut::MesherTest );

namespace fwIGG
{
namespace ut
{

//-----------------------------------------------------------------------------

void MesherTest::setUp()
{
    // Set up context before running a test.
    fwCore::log::SpyLogger& logger = fwCore::log::SpyLogger::getSpyLogger();
    logger.addStreamAppender();
}

//-----------------------------------------------------------------------------

void MesherTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

::fwData::Image::sptr loadInrImage( const ::boost::filesystem::path& imagePath )
{
    // Complete path
    const ::boost::filesystem::path PATH = ::fwTest::Data::dir() / imagePath;

    CPPUNIT_ASSERT_MESSAGE("The file '" + PATH.string() + "' does not exist",
                           ::boost::filesystem::exists(PATH));

    // Load Image
    ::fwData::Image::sptr image           = ::fwData::Image::New();
    ::fwItkIO::ImageReader::sptr myReader = ::fwItkIO::ImageReader::New();
    myReader->setObject(image);
    myReader->setFile(PATH);
    myReader->read();

    return image;
}

//-----------------------------------------------------------------------------

void executeMCMesher( const ::boost::filesystem::path& imagePath)
{
    ::fwData::Image::sptr mask = loadInrImage( imagePath );
    SLM_ASSERT("Mask did not load properly", mask);
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    ::fwIGG::Mesher::computeMeshMC<unsigned char>( mask, mesh, 255, 255 );
}

//-----------------------------------------------------------------------------

void executeMCSMesher( const ::boost::filesystem::path& imagePath, int nbf, int adapt, int radius )
{
    ::fwData::Image::sptr mask = loadInrImage( imagePath );
    ::fwData::Mesh::sptr mesh  = ::fwData::Mesh::New();

    ::fwIGG::Mesher::computeMeshMCS<unsigned char>( mask, mesh, 1, 255, nbf, adapt, radius, false, true );
}

//-----------------------------------------------------------------------------

void MesherTest::memoryTest()
{
#ifdef _WIN32
    executeMCMesher("igg/liver_01_masks_inr/liver.inr.gz");
    executeMCSMesher("igg/classic/bunny.inr.gz", 50000, 100, 5);

    ::boost::uint64_t initialMemory = ::fwMemory::tools::MemoryMonitorTools::getUsedProcessMemory();

    executeMCMesher("igg/liver_01_masks_inr/rightkidney.inr.gz");
    executeMCMesher("igg/liver_01_masks_inr/artery.inr.gz");
    executeMCMesher("igg/liver_01_masks_inr/portalvein.inr.gz");
    executeMCMesher("igg/liver_01_masks_inr/venoussystem.inr.gz");

    executeMCSMesher("igg/classic/happy.inr.gz", 300000, 100, 5);
    executeMCSMesher("igg/classic/dragon.inr.gz", 100000, 100, 5);
    executeMCSMesher("igg/classic/happy.inr.gz", 120000, 100, 5);

    ::boost::uint64_t finalMemory = ::fwMemory::tools::MemoryMonitorTools::getUsedProcessMemory();

    SLM_TRACE("Initial used memory: " + boost::lexical_cast<std::string>(initialMemory) +
              "   Final used memory : " + boost::lexical_cast<std::string>(finalMemory));
    CPPUNIT_ASSERT(initialMemory > finalMemory - 5000000);
#endif //_WIN32
}

//-----------------------------------------------------------------------------

void MesherTest::sameMeshTest()
{
    ::fwData::Image::sptr mask = loadInrImage("igg/classic/sphere.inr.gz");

    ::fwData::Mesh::sptr mesh1 = ::fwData::Mesh::New();
    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();

    unsigned int nbf    = 200000;
    unsigned int adapt  = 100;
    unsigned int radius = 5;

    bool percent = false;
    bool closing = true;

    ::fwIGG::Mesher::computeMeshMCS<unsigned char>( mask, mesh1, 1, 255, nbf, adapt, radius, percent, closing );
    ::fwIGG::Mesher::computeMeshMCS<unsigned char>( mask, mesh2, 255, 255, nbf, adapt, radius, percent, closing );

    ::fwData::Array::sptr pointsMesh1 = mesh1->getPointsArray();
    ::fwData::Array::sptr pointsMesh2 = mesh2->getPointsArray();

    CPPUNIT_ASSERT_EQUAL(pointsMesh1->getSize()[0], pointsMesh2->getSize()[0]);

    ::fwDataTools::helper::Array::sptr helperPointsMesh1 = ::fwDataTools::helper::Array::New(pointsMesh1);
    ::fwDataTools::helper::Array::sptr helperPointsMesh2 = ::fwDataTools::helper::Array::New(pointsMesh2);

    ::fwData::Mesh::PointValueType* points1 = helperPointsMesh1->begin< ::fwData::Mesh::PointValueType >();
    ::fwData::Mesh::PointValueType* points2 = helperPointsMesh2->begin< ::fwData::Mesh::PointValueType >();

    int size = pointsMesh1->getSize()[0];

    float xValue1 = 0;
    float xValue2 = 0;
    float yValue1 = 0;
    float yValue2 = 0;
    float zValue1 = 0;
    float zValue2 = 0;

    for(int i = 0; i<size/3; ++i)
    {
        xValue1 += points1[i];
        xValue2 += points2[i];

        yValue1 += points1[i+1];
        yValue2 += points2[i+1];

        zValue1 += points1[i+2];
        zValue2 += points2[i+2];

        points1 += 3;
        points2 += 3;
    }

    xValue1 /= size;
    xValue2 /= size;

    yValue1 /= size;
    yValue2 /= size;

    zValue1 /= size;
    zValue2 /= size;

    CPPUNIT_ASSERT_EQUAL(xValue1, xValue2);
    CPPUNIT_ASSERT_EQUAL(yValue1, yValue2);
    CPPUNIT_ASSERT_EQUAL(zValue1, zValue2);
}


//-----------------------------------------------------------------------------



} //namespace ut
} //namespace fwIGG
