/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CsvReaderTest.hpp"

#include <fwCsvIO/CsvReader.hpp>

#include <fwTools/System.hpp>

#include <fstream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCsvIO::ut::CsvReaderTest );

namespace fwCsvIO
{
namespace ut
{

//------------------------------------------------------------------------------

void CsvReaderTest::setUp()
{
    // Set up context before running a test.
    // Create a temporary file path for testing
    m_tmpCsvFilePath = (::fwTools::System::getTemporaryFolder() / "TestFile.csv").string();

    std::fstream file;

    file.open(m_tmpCsvFilePath.c_str(), std::fstream::out );

    // test if file created properly
    CPPUNIT_ASSERT(file);

    file.close();
}

//------------------------------------------------------------------------------

void CsvReaderTest::tearDown()
{
    // Clean up after the test run.
    bool deleteStatus = (std::remove(m_tmpCsvFilePath.c_str()) != 0);

    // test if the file correctly removed
    CPPUNIT_ASSERT(!deleteStatus);
}

//------------------------------------------------------------------------------

void CsvReaderTest::testParsing()
{
    // create the CSVFileValuesType to test
    CsvReader::CSVFileValuesType testVector;
    CsvReader::CSVLineValuesType testVector1;
    CsvReader::CSVLineValuesType testVector2;
    CsvReader::CSVLineValuesType testVector3;

    // we tried to create every ambigous case possible.
    testVector1.push_back("");
    testVector1.push_back("f4s");
    testVector1.push_back("is");
    testVector1.push_back("cool");
    // This value needs to be added manually as there is no way to know where the double quotes were placed initially
    testVector1.push_back("FrameWork For Software \n Production Line");
    testVector1.push_back("");
    testVector1.push_back("3.14159265358979323846264338327");
    testVector1.push_back("");

    testVector2.push_back("This is a String with \" quotes \".");
    testVector2.push_back("Ajar JarJar in a Jar in an ajar jar.");
    testVector2.push_back("42");

    testVector3.push_back("The princess is in another castle. ");
    testVector3.push_back("123456789");
    testVector3.push_back("0 0 1 2 3 5 8 13 This is a \" fibonacci \" sequence ");

    testVector.push_back(testVector1);
    testVector.push_back(testVector2);
    testVector.push_back(testVector3);

    // open our test file
    std::fstream file;

    file.open(m_tmpCsvFilePath.c_str(), std::ios::out );

    const size_t testVectorSize = testVector.size();

    // Push values to our file
    for(size_t i = 0; i < testVectorSize; i++)
    {
        const size_t tokenSize = testVector[i].size();

        for(size_t j = 0; j < tokenSize; j++)
        {
            // this line has to be added manually.
            if( i == 0 && j == 4 )
            {
                file<< "FrameWork For \"Software \n\" Production Line,";
            }
            else
            {
                std::string value = testVector[i][j];
                size_t startPos   = 0;
                while( (startPos = value.find( "\"", startPos ) ) != std::string::npos)
                {
                    value.replace(startPos, 1, "\"\"");
                    startPos += 2;
                }
                while( (startPos = value.find( "\n", startPos ) ) != std::string::npos)
                {
                    value.replace(startPos, 1, "\"\"");
                    startPos += 2;
                }

                file<<value;

                if(j != testVector[i].size()-1)
                {
                    file<<",";
                }
            }
        }

        if(i != testVector.size()-1)
        {
            file<<"\n";
        }
    }

    // pushing is done, we may close our file.
    file.close();

    // call our library method
    ::fwCsvIO::CsvReader testReader(m_tmpCsvFilePath);

    CsvReader::CSVFileValuesType resultVector = testReader.read();

    // both vectors should have the same size
    CPPUNIT_ASSERT_EQUAL(testVector.size(), resultVector.size());

    // test our library method
    for(size_t i = 0; i < testVectorSize; i++)
    {
        CPPUNIT_ASSERT_EQUAL(testVector[i].size(), resultVector[i].size());

        const size_t tokenSize = testVector[i].size();

        for(size_t j = 0; j < tokenSize; j++)
        {
            CPPUNIT_ASSERT_EQUAL(testVector[i][j],resultVector[i][j]);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace fwCsvIO
