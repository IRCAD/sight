/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_UT_DIRTEST_HPP__
#define __FWZIP_UT_DIRTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwZip
{
namespace ut
{

class DirTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DirTest );
CPPUNIT_TEST( writeReadFileTest );
CPPUNIT_TEST( writeDirTest );
CPPUNIT_TEST( putFileTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadFileTest();
    void writeDirTest();
    void putFileTest();
};

} // namespace ut
} // namespace fwZip

#endif //__FWZIP_UT_DIRTEST_HPP__
