/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_TEST_TU_DICTIONARYREADERTEST_HPP_
#define _FWDATAIO_TEST_TU_DICTIONARYREADERTEST_HPP_

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

/**
 * @brief Test many methods to read/create fw4spl dictionary file.
 */
class DictionaryReaderTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DictionaryReaderTest );
    CPPUNIT_TEST( test_1 );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void test_1();

protected:

    void generateWrongDictionaryFile(::boost::filesystem::path dictionaryFile);
    void generateDictionaryFile(::boost::filesystem::path dictionaryFile);
    ::boost::filesystem::path m_tmpDictionaryFilePath;
};

#endif //_FWDATAIO_TEST_TU_DICTIONARYREADERTEST_HPP_
