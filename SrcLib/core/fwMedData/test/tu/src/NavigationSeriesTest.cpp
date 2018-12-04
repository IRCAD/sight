/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "NavigationSeriesTest.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/Writer.hpp>
#include <fwAtomsBoostIO/types.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::NavigationSeriesTest );

namespace fwMedData
{
namespace ut
{

::fwAtoms::Object::sptr readNavigation( const ::boost::filesystem::path& filePath )
{
    const ::boost::filesystem::path folderPath = filePath.parent_path();
    const ::boost::filesystem::path filename   = filePath.filename();
    const std::string extension                = ::boost::filesystem::extension(filePath);

    ::fwZip::IReadArchive::sptr readArchive;
    ::boost::filesystem::path archiveRootName;
    ::fwAtomsBoostIO::FormatType format = ::fwAtomsBoostIO::UNSPECIFIED;

    if ( extension == ".json" )
    {
        readArchive     = ::fwZip::ReadDirArchive::New(folderPath.string());
        archiveRootName = filename;
        format          = ::fwAtomsBoostIO::JSON;
    }
    else if ( extension == ".jsonz" )
    {
        readArchive     = ::fwZip::ReadZipArchive::New(filePath.string());
        archiveRootName = "root.json";
        format          = ::fwAtomsBoostIO::JSON;
    }
    else
    {
        FW_RAISE( "This file extension '" << extension << "' is not managed" );
    }

    ::fwAtomsBoostIO::Reader reader;
    ::fwAtoms::Object::sptr atom =
        ::fwAtoms::Object::dynamicCast(reader.read(readArchive, archiveRootName, format));

    return atom;
}

//------------------------------------------------------------------------------

void writeNavigation( const ::boost::filesystem::path& filePath, const ::fwAtoms::Object::sptr atom )
{
    const ::boost::filesystem::path folderPath = filePath.parent_path();
    const ::boost::filesystem::path filename   = filePath.filename();
    std::string extension                      = ::boost::filesystem::extension(filePath);

    // Write atom
    ::fwZip::IWriteArchive::sptr writeArchive;
    ::fwAtomsBoostIO::FormatType format;
    ::boost::filesystem::path archiveRootName;

    if ( extension == ".json" )
    {
        writeArchive    = ::fwZip::WriteDirArchive::New(folderPath.string());
        archiveRootName = filename;
        format          = ::fwAtomsBoostIO::JSON;
    }
    else if ( extension == ".jsonz" )
    {
        if ( ::boost::filesystem::exists( filePath ) )
        {
            ::boost::filesystem::remove( filePath );
        }
        writeArchive    = ::fwZip::WriteZipArchive::New(filePath.string());
        archiveRootName = "root.json";
        format          = ::fwAtomsBoostIO::JSON;
    }
    else
    {
        FW_RAISE( "This file extension '" << extension << "' is not managed" );
    }

    ::fwAtomsBoostIO::Writer(atom).write( writeArchive, archiveRootName, format );
    writeArchive.reset();
}

//------------------------------------------------------------------------------

void NavigationSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::NavigationSeries::New();
}

//------------------------------------------------------------------------------

void NavigationSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void NavigationSeriesTest::navigationTest()
{
    CPPUNIT_ASSERT(m_series);
    ::fwMedData::NavigationSeries::ContainerType positions =
    {
        {0., {-514.1606513019431, -94.68116795798758, 979.3785055174628}}
    };

    ::fwMedData::NavigationSeries::ContainerType move =
    {
        {0., {0.897820633950464, 0.13607544320686907, -1.2769419285130967}},
        {1421.375, {2.322431935080602, 0.33708203932328956, -3.734284956660633}},
        {2114.208, {2.43713135678714, -0.049220088700801926, -3.266794030753196}},
        {3377.9159999999997, {0., 0., 0.}},
        {4027.6536502546687, {-3.964310130780936, 0.08006285575503613, 5.313864037415808}},
        {6237.149579831932, {0., 0., 0.}},
        {6500.074702886248, {-0.8058588959846187, 1.6088833889257592, -2.2406369414514837}},
        {7616.651952461799, {0.15881288125106394, 0.9634983958585909, -3.792280054764281}},
        {9291.517826825127, {15.00876504273313, 1.948698699541007, 9.621351972786318}},
        {10328.339558573854, {8.746762102220163, -1.3252231011236748, 0.40990799919194243}},
        {11344.944, {3.107379091978582, 0.24758149132857085, -1.7704233405817775}},
        {13717.94906621392, {-14.868514648384936, -0.06864726970614742, -1.8512522029189045}},
        {14874.404074702887, {-2.1505985395333633, 0.0025699595738454374, -0.5800507110513734}},
        {15472.570458404074, {0.5463244285267206, -0.009449336194383946, -4.067785389892101}},
        {18862.179966044143, {0., 0., 0.}},
        {20576.923599320886, {-1.4763543428185393, 0.6215147468157091, 4.983670944496179}},
        {23208.855687606112, {0., 0., 0.}}
    };

    ::fwMedData::NavigationSeries::ContainerType lookAt =
    {
        {0., {216.48659596562175, -109.43725495319805, 0.}},
        {4027.6536502546687, {206.6271751068543, -90.3143606602177, -7.6435594203111314}},
        {6500.074702886248, {206.6271751068543, -90.3143606602177, -7.6435594203111314}}
    };

    // Test add API
    for(const auto& elt : positions)
    {
        m_series->addPosition(elt.first, elt.second);
    }
    for(const auto& elt : move)
    {
        m_series->addMove(elt.first, elt.second);
    }
    for(const auto& elt : lookAt)
    {
        m_series->addLookAt(elt.first, elt.second);
    }
    CPPUNIT_ASSERT_EQUAL(positions.size(), m_series->getPositionMap().size());
    CPPUNIT_ASSERT_EQUAL(move.size(), m_series->getMoveMap().size());
    CPPUNIT_ASSERT_EQUAL(lookAt.size(), m_series->getLookAtMap().size());

    CPPUNIT_ASSERT(positions == m_series->getPositionMap());
    CPPUNIT_ASSERT(move == m_series->getMoveMap());
    CPPUNIT_ASSERT(lookAt == m_series->getLookAtMap());

    // Create Path
    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "navigationtest";
    ::boost::filesystem::create_directories(path);

    const std::string jsonzFile = "NavigationSeriesTest.jsonz";
    ::fwAtoms::Object::sptr atom1 = ::fwAtomConversion::convert(m_series);
    writeNavigation((path/jsonzFile), atom1);

    ::fwAtoms::Object::sptr atom2 = readNavigation(path/jsonzFile);

    ::fwData::Object::sptr object;
    ::fwMedData::NavigationSeries::sptr navigationSeries;

    object           = ::fwAtomConversion::convert(atom2, ::fwAtomConversion::AtomVisitor::ChangePolicy());
    navigationSeries = ::fwMedData::NavigationSeries::dynamicCast(object);

    // Test get API
    CPPUNIT_ASSERT(navigationSeries);
    CPPUNIT_ASSERT_EQUAL(positions.size(), navigationSeries->getPositionMap().size());
    CPPUNIT_ASSERT_EQUAL(move.size(), navigationSeries->getMoveMap().size());
    CPPUNIT_ASSERT_EQUAL(lookAt.size(), navigationSeries->getLookAtMap().size());

    CPPUNIT_ASSERT(positions == navigationSeries->getPositionMap());
    CPPUNIT_ASSERT(move == navigationSeries->getMoveMap());
    CPPUNIT_ASSERT(lookAt == navigationSeries->getLookAtMap());

    // Test erase API
    double timestamp = positions.begin()->first;
    navigationSeries->erasePosition(timestamp);
    CPPUNIT_ASSERT(navigationSeries->getPositionMap().find(timestamp) == navigationSeries->getPositionMap().end());

    timestamp = move.begin()->first;
    navigationSeries->eraseMove(timestamp);
    CPPUNIT_ASSERT(navigationSeries->getMoveMap().find(timestamp) == navigationSeries->getMoveMap().end());

    timestamp = lookAt.begin()->first;
    navigationSeries->eraseLookAt(timestamp);
    CPPUNIT_ASSERT(navigationSeries->getLookAtMap().find(timestamp) == navigationSeries->getLookAtMap().end());

    // Test clear API
    navigationSeries->clearPosition();
    CPPUNIT_ASSERT(navigationSeries->getPositionMap().empty());

    navigationSeries->clearMove();
    CPPUNIT_ASSERT(navigationSeries->getMoveMap().empty());

    navigationSeries->clearLookAt();
    CPPUNIT_ASSERT(navigationSeries->getLookAtMap().empty());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedData
