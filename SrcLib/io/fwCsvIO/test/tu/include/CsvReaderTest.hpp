/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCSVIO_UT_CSVREADERTEST_HPP__
#define __FWCSVIO_UT_CSVREADERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCsvIO
{

namespace ut
{


/**
 * @brief Test csvReader method to read CSV formated files and create vectors of strings.
 * Note : Our tests are done considering that a csvFile cannot contain errors. (E.g. in case of two consecutive commas
 * string is considered empty for example).
 */
class CsvReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( CsvReaderTest );

CPPUNIT_TEST( testParsing );

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();

    void tearDown();

    void testParsing();

protected:

    /// string containing the path to the temporary file to test
    std::string m_tmpCsvFilePath;

};

} // namespace ut

} // namespace fwCsvIO

#endif //__FWCSVIO_UT_CSVREADERTEST_HPP__
