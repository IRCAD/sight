/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/core/config.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <doctest/doctest.h>

#include <string>
#include <utility>

namespace sight::utest
{

/**
 * @brief Fixture interface for service unit tests
 *
 * This fixture provides a service instance and a worker for the tests.
 * It ensures proper cleanup of the service and worker after each test, whether the test passes or fails.
 * It also provides a helper method to create slots with the worker set.
 * Usage:
 * - Inherit from service_fixture in your test fixture class.
 * - Call the base constructor with the service implementation name.
 * - Use m_service to access the service instance.
 */
class service_fixture
{
public:

    //------------------------------------------------------------------------------

    virtual ~service_fixture()
    {
        if(m_service->started())
        {
            CHECK_NOTHROW(m_service->stop().get());
        }

        sight::service::remove(m_service);

        if(m_worker)
        {
            m_worker->stop();
        }
    }

    //------------------------------------------------------------------------------

    template<typename L>
    [[nodiscard]] auto new_slot(L&& _lambda)
    {
        auto slot = sight::core::com::new_slot(std::forward<L>(_lambda));
        if(!m_worker)
        {
            m_worker = sight::core::thread::worker::make();
        }

        slot->set_worker(m_worker);

        return slot;
    }

    // We assume this to be public for convenience
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    sight::service::base::sptr m_service;

protected:

    //------------------------------------------------------------------------------

    explicit service_fixture(std::string  _impl)
    {
        m_service = sight::service::add(_impl);
        REQUIRE(m_service);
        REQUIRE(m_service->is_a(_impl));
    }

private:

    sight::core::thread::worker::sptr m_worker;
};

} // namespace sight::utest
