/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DATA_PATIENTTEST_H
#define DATA_PATIENTTEST_H

#include <cppunit/extensions/HelperMacros.h>

class PatientTest : public CPPUNIT_NS::TestFixture {
        CPPUNIT_TEST_SUITE( PatientTest );
        CPPUNIT_TEST( methodes1 );
        CPPUNIT_TEST( methodes2 );
        CPPUNIT_TEST( addStudy );
        CPPUNIT_TEST_SUITE_END();
public:
        // interface
        void setUp();
        void tearDown();
        // fonctions de tests
        void methodes1();
        void methodes2(); //setters différents
        void addStudy() ;
};
#endif
