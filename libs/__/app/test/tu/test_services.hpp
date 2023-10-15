/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/base.hpp>

#include <data/image.hpp>

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

/**
 * @brief   Service type for test.
 */
class TestConfigService : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(TestConfigService, service::base);

    ~TestConfigService() noexcept override =
        default;

    /// return true if the service is updated with updating() method
    bool getIsUpdated() const
    {
        return m_isUpdated;
    }

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    //------------------------------------------------------------------------------

    void info(std::ostream& _sstream) override
    {
        _sstream << "TestConfigService";
    }

    bool m_isUpdated {false};
};

/**
 * @brief   Test service implementation for image
 */
class STest1Image : public TestConfigService
{
public:

    SIGHT_DECLARE_SERVICE(STest1Image, app::ut::TestConfigService);

    ~STest1Image() noexcept override =
        default;

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
        m_isUpdated = true;
    }

    data::ptr<data::image, data::Access::in> m_input {this, "data", true, true};
};

} // namespace sight::app::ut
