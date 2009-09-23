/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TEST_TU_MESSAGESTEST_HPP_
#define _FWCOMED_TEST_TU_MESSAGESTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

/**
 * @brief 	Test specified object messages.
 */
class MessagesTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( MessagesTest );
	CPPUNIT_TEST( testFloatMsg );
	CPPUNIT_TEST( testImageMsg );
	CPPUNIT_TEST( testPatientDBMsg );
	CPPUNIT_TEST_SUITE_END();

public:
	// interface
	void setUp();
	void tearDown();

	// fonctions de tests

	/// test FloatMsg
	void testFloatMsg();

	/// test ImageMsg
	void testImageMsg();

	/// test PatientDBMsg
	void testPatientDBMsg();
};


#endif // _FWCOMED_TEST_TU_MESSAGESTEST_HPP_
