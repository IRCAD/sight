/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_OBJECTSERVICETEST_HPP__
#define __FWSERVICES_UT_OBJECTSERVICETEST_HPP__

#include <fwData/Object.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <condition_variable>
#include <mutex>

namespace fwServices
{
namespace ut
{

/**
 * @brief
 */
class ObjectServiceTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ObjectServiceTest );
CPPUNIT_TEST( swapTest );
CPPUNIT_TEST( registerTest );
CPPUNIT_TEST( registerKeyTest );
CPPUNIT_TEST( registerConnectionTest );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void swapTest();
    void registerTest();
    void registerKeyTest();
    void registerConnectionTest();

private:
    void registerService(::fwData::Object::sptr obj, const std::string& id);
    void unregisterService(::fwData::Object::sptr obj, const std::string& id);

    /// Synchronization with slots
    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::string m_ret;
    ::fwData::Object::csptr m_obj;
};

} //namespace ut
} //namespace fwServices

#endif //__FWSERVICES_UT_OBJECTSERVICETEST_HPP__
