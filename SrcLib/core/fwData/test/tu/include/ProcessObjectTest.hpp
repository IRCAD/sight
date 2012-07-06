/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_PROCESSOBJECTTEST_HPP__
#define __FWDATA_UT_PROCESSOBJECTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class ProcessObjectTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ProcessObjectTest );
    CPPUNIT_TEST( constructeur );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp(){};
    void tearDown(){};

    void constructeur();
};
} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_PROCESSOBJECTTEST_HPP__
