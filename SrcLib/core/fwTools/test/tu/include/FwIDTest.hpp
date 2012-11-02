/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_UT_FWIDTEST_HPP__
#define __FWTOOLS_UT_FWIDTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwTools
{
namespace ut
{

class FwIDTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( FwIDTest );
    CPPUNIT_TEST( objectFwIDTest );
    CPPUNIT_TEST( conccurentAccessOnFwIDMapTest );
    CPPUNIT_TEST( conccurentAccessOnSameObjFwIDTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void objectFwIDTest();

    void conccurentAccessOnFwIDMapTest();
    void conccurentAccessOnSameObjFwIDTest();

private:
    void runFwIDCreation();
    void runAccessToObjectFwID();

    ::fwTools::Object::sptr m_object;
};

} // namespace ut
} // namespace fwTools

#endif //__FWTOOLS_UT_FWIDTEST_HPP__
