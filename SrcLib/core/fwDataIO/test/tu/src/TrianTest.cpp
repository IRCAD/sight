/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/regex.hpp>

#include <fwTools/System.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataIO/reader/TriangularMeshReader.hpp>
#include <fwDataIO/writer/TriangularMeshWriter.hpp>
#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwTest/Data.hpp>

#include "FileNameParser.hpp"
#include "TrianTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataIO::ut::TrianTest );

namespace fwDataIO
{
namespace ut
{

static const unsigned int NBPTS   = 100;
static const unsigned int NBCELLS = 100;

//------------------------------------------------------------------------------

void TrianTest::setUp()
{
    // Set up context before running a test.
    m_tmpTrianPath1 = ::fwTools::System::getTemporaryFolder() / "TrianTest1.trian";
    m_tmpTrianPath2 = ::fwTools::System::getTemporaryFolder() / "TrianTest2.trian";

    this->generateTrian(m_tmpTrianPath1);

    CPPUNIT_ASSERT(::boost::filesystem::exists(m_tmpTrianPath1));
}

//------------------------------------------------------------------------------

void TrianTest::tearDown()
{
    // Clean up after the test run.
    if( ::boost::filesystem::exists(m_tmpTrianPath1))
    {
        bool suppr = ::boost::filesystem::remove(m_tmpTrianPath1);
        CPPUNIT_ASSERT(suppr);
    }
    if( ::boost::filesystem::exists(m_tmpTrianPath2))
    {
        bool suppr = ::boost::filesystem::remove(m_tmpTrianPath2);
        CPPUNIT_ASSERT(suppr);
    }
}

//------------------------------------------------------------------------------

void TrianTest::test_1()
{
    ::fwData::TriangularMesh::sptr trianMesh = ::fwData::TriangularMesh::New();
    ::fwDataIO::reader::TriangularMeshReader::sptr reader = ::fwDataIO::reader::TriangularMeshReader::New();

    reader->setObject(trianMesh);
    reader->setFile(m_tmpTrianPath1);
    reader->read();

    CPPUNIT_ASSERT_EQUAL((size_t)NBPTS, trianMesh->getNumPoints());
    CPPUNIT_ASSERT_EQUAL((size_t)NBCELLS, trianMesh->getNumCells());
}

//------------------------------------------------------------------------------

void TrianTest::test_2()
{
    ::fwData::TriangularMesh::sptr trianMesh1 = ::fwData::TriangularMesh::New();
    ::fwData::TriangularMesh::sptr trianMesh2 = ::fwData::TriangularMesh::New();

    ::fwDataIO::reader::TriangularMeshReader::sptr reader = ::fwDataIO::reader::TriangularMeshReader::New();
    ::fwDataIO::writer::TriangularMeshWriter::sptr writer = ::fwDataIO::writer::TriangularMeshWriter::New();

    reader->setObject(trianMesh1);
    reader->setFile(m_tmpTrianPath1);
    reader->read();

    writer->setObject(trianMesh1);
    writer->setFile(m_tmpTrianPath2);
    writer->write();

    reader->setObject(trianMesh2);
    reader->setFile(m_tmpTrianPath2);
    reader->read();

    CPPUNIT_ASSERT_EQUAL((size_t)NBPTS, trianMesh1->getNumPoints());
    CPPUNIT_ASSERT_EQUAL((size_t)NBCELLS, trianMesh1->getNumCells());
    CPPUNIT_ASSERT_EQUAL(trianMesh1->getNumCells(),  trianMesh2->getNumCells());
    CPPUNIT_ASSERT_EQUAL(trianMesh1->getNumPoints(), trianMesh2->getNumPoints());
    CPPUNIT_ASSERT(trianMesh1->points() == trianMesh2->points());
    CPPUNIT_ASSERT(trianMesh1->cells()  == trianMesh2->cells());
}

//------------------------------------------------------------------------------

void TrianTest::file_load_test()
{

    // test parseMeshFileName
    unsigned long long nbPts, nbCells;
    std::string testFileName("Mesh-00-002502pts-005000cells.trian");
    parseMeshFileName( testFileName, nbPts, nbCells );
    CPPUNIT_ASSERT_EQUAL(2502ULL, nbPts);
    CPPUNIT_ASSERT_EQUAL(5000ULL, nbCells);

    const ::boost::filesystem::path trian_path( ::fwTest::Data::dir() / "fw4spl/trian/" );
    const boost::regex fileNameFilter( "Mesh-\\d+-\\d+pts-\\d+cells\\.trian" );

    typedef std::vector< ::boost::filesystem::path > FileVector;
    FileVector files;

    // find every files matching <fileNameFilter> pattern
    boost::filesystem::directory_iterator iter( trian_path );
    boost::filesystem::directory_iterator end_itr;
    boost::smatch what;
    for( ; iter != end_itr; ++iter )
    {
        if( !boost::filesystem::is_regular_file( iter->status() )
                || !boost::regex_match( iter->path().filename().string(), what, fileNameFilter ) )
        {
            continue;
        }
        files.push_back( iter->path() );
    }

    CPPUNIT_ASSERT_MESSAGE("Missing trian files", !files.empty() );

    FileVector::const_iterator fIter = files.begin();
    FileVector::const_iterator fEndIter = files.end();

    for (; fIter != fEndIter; ++fIter)
    {
        ::boost::filesystem::path trianFile = *fIter;
        OSLM_TRACE("Testing: " << trianFile);

        CPPUNIT_ASSERT_MESSAGE(
                "Failed parsing mesh filename" + trianFile.filename().string(),
                parseMeshFileName( trianFile.filename().string(), nbPts, nbCells )
                );

        ::fwData::TriangularMesh::sptr trianMesh = ::fwData::TriangularMesh::New();
        ::fwDataIO::reader::TriangularMeshReader::sptr reader = ::fwDataIO::reader::TriangularMeshReader::New();

        reader->setObject(trianMesh);
        reader->setFile(trianFile);
        reader->read();

        CPPUNIT_ASSERT_EQUAL_MESSAGE(trianFile.string(), (size_t)nbPts, trianMesh->getNumPoints());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("File : " +  trianFile.string(), (size_t)nbCells, trianMesh->getNumCells());
    }
}
//------------------------------------------------------------------------------

void TrianTest::generateTrian(::boost::filesystem::path trianFile)
{
    std::fstream file;
    file.open(trianFile.string().c_str(), std::fstream::out);
    CPPUNIT_ASSERT(file.is_open());

    unsigned int i;
    file<<NBPTS<<std::endl;
    for( i=0 ; i<NBPTS ; ++i )
    {
        file<<i<<" "<<i+1<<" "<<i+2<<std::endl;
    }

    file<<NBCELLS<<std::endl;
    for( i=0 ; i<NBCELLS ; ++i )
    {
        file<<i%NBPTS<<" "<<(i+1)%NBPTS<<" "<<(i+2)%NBPTS<<" "<<-1<<" "<<-1<<" "<<-1<<std::endl;
    }

    file.close();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataIO
