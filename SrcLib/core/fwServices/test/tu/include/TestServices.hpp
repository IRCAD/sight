/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
