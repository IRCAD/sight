/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

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
    fwCoreServiceClassDefinitionsMacro( (TestConfigService)(::fwServices::IService) )
    TestConfigService() noexcept :
        m_isUpdated(false)
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
    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void updating() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void info( std::ostream& _sstream ) override
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
    fwCoreServiceClassDefinitionsMacro( (TestServiceImplementationImage)(::fwServices::ut::TestConfigService) )
    TestServiceImplementationImage() noexcept
    {
    }
    virtual ~TestServiceImplementationImage() noexcept
    {
    }

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void updating() override
    {
        m_isUpdated = true;
    }

};

} //namespace ut
} //namespace fwServices
