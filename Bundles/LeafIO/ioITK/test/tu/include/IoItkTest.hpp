/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __IOITK_TEST_TU_IOITKTEST_HPP__
#define __IOITK_TEST_TU_IOITKTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace fwData
{
    class Object;
}

namespace fwRuntime
{
    class EConfigurationElement;
}

namespace ioITK
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class IoItkTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoItkTest );
    CPPUNIT_TEST( testImageWriterJPG );
    CPPUNIT_TEST( testImageSeriesWriterJPG );
    CPPUNIT_TEST( testPatientDBReaderJPG );
    CPPUNIT_TEST( testSaveLoadInr );
    CPPUNIT_TEST( testLoadInr );
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void testImageWriterJPG();
    void testImageSeriesWriterJPG();
    void testPatientDBReaderJPG();
    void testSaveLoadInr();
    void testLoadInr();
private :

    void executeService(
            const SPTR(::fwData::Object)& obj,
            const std::string& srvType,
            const std::string& srvImpl,
            const SPTR(::fwRuntime::EConfigurationElement)& cfg);

};

} //namespace ut
} //namespace ioITK

#endif //__IOITK_TEST_TU_IOITKTEST_HPP__

