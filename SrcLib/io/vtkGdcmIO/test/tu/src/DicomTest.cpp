/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fwTools/System.hpp>

#include <fwData/PatientDB.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/ObjectGenerator.hpp>
#include <fwDataTools/ObjectComparator.hpp>

#include <vtkGdcmIO/DicomPatientDBWriter.hpp>
#include <vtkGdcmIO/DicomPatientDBReader.hpp>

#include "DicomTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::vtkGdcmIO::ut::DicomTest );

namespace vtkGdcmIO
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomTest::methode1()
{
    setlocale(LC_NUMERIC,"C");

    // create patientDB
    ::fwData::PatientDB::sptr pPatientDB;
    pPatientDB = ::fwDataTools::ObjectGenerator::createPatientDB(1, 2, 2, 0);

    // save patientDB in fwXML
    const ::boost::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "dicomTest";

    ::boost::filesystem::create_directories( PATH );

    ::vtkGdcmIO::DicomPatientDBWriter::NewSptr writer;
    writer->setObject(pPatientDB);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load patientDB
    ::fwData::PatientDB::NewSptr pPatientDB2;
    ::vtkGdcmIO::DicomPatientDBReader::NewSptr reader;
    reader->setObject(pPatientDB2);
    reader->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    ::boost::filesystem::remove_all( PATH.string() );

    // check patientDB
    CPPUNIT_ASSERT(pPatientDB2);
    CPPUNIT_ASSERT_EQUAL(pPatientDB->getNumberOfPatients(), pPatientDB2->getNumberOfPatients());

    CPPUNIT_ASSERT(::fwDataTools::ObjectComparator::comparePatientDB(pPatientDB, pPatientDB2));

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace vtkGdcmIO
