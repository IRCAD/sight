/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOXML_TU_READERWRITERTEST_HPP_
#define _IOXML_TU_READERWRITERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

namespace ioXML
{
namespace ut
{

/**
 * @brief Test fwXML reader/writer.
 */
class ReaderWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ReaderWriterTest );
    CPPUNIT_TEST( testReader );
    CPPUNIT_TEST( testWriter );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testReader();
    void testWriter();
};

} //namespace ut
} //namespace ioXML

#endif //_IOXML_TU_READERWRITERTEST_HPP_
