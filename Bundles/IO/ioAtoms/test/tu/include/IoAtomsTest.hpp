/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_UT_IOATOMSTEST_HPP__
#define __IOATOMS_UT_IOATOMSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioAtoms
{
namespace ut
{

/**
 * @brief Test basic reader/writer for SeriesDB.
 */
class IoAtomsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IoAtomsTest );
CPPUNIT_TEST( JSONTest );
CPPUNIT_TEST( JSONZTest );
CPPUNIT_TEST( XMLTest );
CPPUNIT_TEST( XMLZTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void JSONTest();
    void JSONZTest();
    void XMLTest();
    void XMLZTest();
};

} // namespace ut
} // namespace ioAtoms

#endif // __IOATOMS_UT_IOATOMSTEST_HPP__
