/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_OBJECTMSGTEST_HPP_
#define _FWSERVICES_TEST_TU_OBJECTMSGTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

/**
 * @brief	Test ObjectMsg API
 */
class ObjectMsgTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( ObjectMsgTest );
	CPPUNIT_TEST( testObjectMsg );
	CPPUNIT_TEST_SUITE_END();


public:
	// interface
	void setUp();
	void tearDown();
	// fonctions de tests

	/// Test using ObjectMsg API
	void testObjectMsg();

};

#endif
