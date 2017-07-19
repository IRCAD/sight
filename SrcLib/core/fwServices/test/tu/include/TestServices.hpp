/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_TESTSERVICES_HPP__
#define __FWSERVICES_UT_TESTSERVICES_HPP__

#include <fwServices/macros.hpp>

#include <fwCore/base.hpp>

#include <cppunit/extensions/HelperMacros.h>


namespace fwServices
{
namespace ut
{

/**
 * @brief   Service type for test.
 */
class TestConfigService : public ::fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (TestConfigService)(::fwServices::IService) );
    TestConfigService() noexcept
        :   m_isUpdated(false)
    {
    }

    virtual ~TestConfigService() noexcept
    {
    }

    /// return true if the service is updated with updating() method
    bool getIsUpdated()
    {
        return m_isUpdated;
    }

protected:
    virtual void configuring()
    {
    }
    virtual void starting()
    {
    }
    virtual void stopping()
    {
    }
    virtual void updating()
    {
    }
    virtual void info( std::ostream& _sstream )
    {
        _sstream << "TestConfigService";
    }

    bool m_isUpdated;
};

/**
 * @brief   Test service implementation for image
 */
class TestServiceImplementationImage : public TestConfigService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationImage)(::fwServices::ut::TestConfigService) );
    TestServiceImplementationImage() noexcept
    {
    }
    virtual ~TestServiceImplementationImage() noexcept
    {
    }

    virtual void configuring()
    {
    }
    virtual void starting()
    {
    }
    virtual void stopping()
    {
    }
    virtual void updating()
    {
        m_isUpdated = true;
    }

};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_TESTSERVICES_HPP__
