/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ImageSeriesWriterTest.hpp"

#include <vtkGdcmIO/ImageSeriesWriter.hpp>
#include <vtkGdcmIO/SeriesDBReader.hpp>

#include <fwData/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/SeriesDB.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::vtkGdcmIO::ut::ImageSeriesWriterTest );

namespace vtkGdcmIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void ImageSeriesWriterTest::setUp()
{
    // Set up context before running a test.

    // HACK: force link with fwMedDataCamp. Needed when calling ::fwDataCamp::visitor::CompareObjects::compare.
    m_medDataCampVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
}

//------------------------------------------------------------------------------

void ImageSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageSeriesWriterTest::writeReadTest()
{
    // HACK, we don't want the compiler to optimize 'm_medDataCampVersion' out.
    CPPUNIT_ASSERT_EQUAL(::fwMedDataCamp::Version::s_CURRENT_VERSION, m_medDataCampVersion);

    ::fwTest::generator::Image::initRand();
    ::fwMedData::ImageSeries::sptr imgSeries;
    imgSeries = ::fwTest::generator::SeriesDB::createImageSeries();

    const ::boost::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "dicomTest";

    ::boost::filesystem::create_directories( PATH );

    ::vtkGdcmIO::ImageSeriesWriter::sptr writer = ::vtkGdcmIO::ImageSeriesWriter::New();
    writer->setObject(imgSeries);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    ::fwMedData::SeriesDB::sptr sdb          = ::fwMedData::SeriesDB::New();
    ::vtkGdcmIO::SeriesDBReader::sptr reader = ::vtkGdcmIO::SeriesDBReader::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    ::boost::filesystem::remove_all( PATH );

    // check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    compare(imgSeries, sdb->getContainer().front());
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace vtkGdcmIO
