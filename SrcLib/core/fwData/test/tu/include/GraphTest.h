/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DATA_GRAPHTEST_H
#define DATA_GRAPHTEST_H

#include <cppunit/extensions/HelperMacros.h>

class GraphTest : public CPPUNIT_NS::TestFixture {

	CPPUNIT_TEST_SUITE( GraphTest );
	CPPUNIT_TEST( normalCase );
	CPPUNIT_TEST( limitCase1 );
	CPPUNIT_TEST( limitCase2 );
	CPPUNIT_TEST_SUITE_END();

public:

	// interface
	void setUp();
	void tearDown();

	// fonctions de tests
	void normalCase();
	void limitCase1();
	void limitCase2();
};

#endif
