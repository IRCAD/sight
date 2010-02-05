/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWRUNTIME_TU_RUNTIMETEST_H
#define FWRUNTIME_TU_RUNTIMETEST_H

#include <cppunit/extensions/HelperMacros.h>

/**
 * @brief 	Test Runtime : read the bundles in CommonLib
 */
class RuntimeTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( RuntimeTest );
	CPPUNIT_TEST( testRuntime );
	CPPUNIT_TEST_SUITE_END();

protected:

public:
	// interface
	void setUp();
	void tearDown();
	// fonctions de tests
	void testRuntime();
};

#endif
