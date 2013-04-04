/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSBOOSTIO_UT_WRITERTEST_HPP__
#define __FWATOMSBOOSTIO_UT_WRITERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwAtomsBoostIO
{
namespace ut
{

class BoostIOTest :  public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( BoostIOTest );
    CPPUNIT_TEST( writeTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeTest();
};


}  // namespace ut
}  // namespace fwAtomsBoostIO



#endif // __FWATOMSBOOSTIO_UT_WRITERTEST_HPP__


