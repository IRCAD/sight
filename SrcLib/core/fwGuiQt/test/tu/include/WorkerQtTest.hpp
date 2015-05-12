/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_UT_WORKERQTTEST_HPP__
#define __FWGUIQT_UT_WORKERQTTEST_HPP__

#include <fwCore/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGuiQt
{

struct WorkerQtInstanciator;

namespace ut
{

class WorkerQtTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( WorkerQtTest );

CPPUNIT_TEST(initTest);
CPPUNIT_TEST(twiceInitTest);
CPPUNIT_TEST(basicTest);
CPPUNIT_TEST(postFromInsideTest);
CPPUNIT_TEST(postFromOutsideTest);

CPPUNIT_TEST(basicTimerTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void initTest();
    void twiceInitTest();
    void basicTest();

    void postFromInsideTest();
    void postFromOutsideTest();

    void basicTimerTest();

protected:

    SPTR(::fwThread::Worker) m_worker;
};

} //namespace ut
} //namespace fwGuiQt

#endif //__FWGUIQT_UT_WORKERQTTEST_HPP__

