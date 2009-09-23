/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef DATA_ModelTEST_H
#define DATA_ModelTEST_H

#include <cppunit/extensions/HelperMacros.h>

class ModelTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE( ModelTest );
	CPPUNIT_TEST( methode1 );
	CPPUNIT_TEST_SUITE_END();
protected:

public:
	// interface
	void setUp();
	void tearDown();
	// fonctions de tests
	void methode1();
};
#endif
