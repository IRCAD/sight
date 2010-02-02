/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TEST_TU_CALLBACKTEST_HPP_
#define _FWCOMED_TEST_TU_CALLBACKTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>


/**
 * @brief 	Test Callback and DispatchMessage
 * @see 	::fwComEd::ICallback
 * @see 	::fwServices::ObjectMsg
 * @see		::fwComEd::DispatchMessage
 */
class CallbackTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CallbackTest );
	CPPUNIT_TEST( methodeCallback );
	CPPUNIT_TEST_SUITE_END();

public:
	// interface
	void setUp();
	void tearDown();
	// tests callback
	void methodeCallback();

	// method used by callback
	void testCallback( ::fwServices::ObjectMsg::csptr _msg );
	void testCallback2( ::fwServices::ObjectMsg::csptr _msg );

private:
	bool m_isCallback1;
	bool m_isCallback2;
};


#endif // _FWCOMED_TEST_TU_CALLBACKTEST_HPP_
