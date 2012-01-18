/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_
#define _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Mesh.hpp>

class DicomTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomTest );
    CPPUNIT_TEST( checkPatient );
    CPPUNIT_TEST( checkImage );
    CPPUNIT_TEST( checkReconstruction );
    CPPUNIT_TEST( checkLandmark );
    CPPUNIT_TEST( checkDistance );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void checkPatient();
    void checkImage();
    void checkReconstruction();
    void checkLandmark();
    void checkDistance();

private:
    ::fwData::PatientDB::sptr createPatientDB();
    ::fwData::Mesh::sptr DicomTest::generateMesh();

    ::fwData::PatientDB::sptr m_originalPatientDB;
    ::fwData::PatientDB::sptr m_readedPatientDB;

};

#endif // _FWVTKGDCMIO_TEST_TU_DICOMTEST_HPP_
