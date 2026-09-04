/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <app/updater.hpp>

#include <data/image.hpp>

#include <condition_variable>
#include <mutex>

namespace sight::app::ut
{

/**
 * @brief   Service type for test.
 */
class test_config_service : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_config_service, service::base);

    ~test_config_service() noexcept override = default;

    /// return true if the service is updated with updating() method
    bool is_updated() const
    {
        return m_is_updated;
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

    bool m_is_updated {false}; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

/**
 * @brief   Test service implementation for image
 */
class test1_image : public test_config_service
{
public:

    SIGHT_DECLARE_SERVICE(test1_image, app::ut::test_config_service);

    ~test1_image() noexcept override =
        default;

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
        m_is_updated = true;
    }

public:

    data::ptr<data::image, data::access::in> m_input {this, "data", true};
};

/**
 * @brief Service for updater tests
 */
class test_order_srv final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_order_srv, service::base);

    static unsigned int s_order;

    //------------------------------------------------------------------------------

    ~test_order_srv() noexcept final = default;

    //------------------------------------------------------------------------------

    unsigned int update_order() const
    {
        return m_update_order;
    }

protected:

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*unused*/) final
    {
    }

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
        m_update_order = s_order++;
    }

    unsigned int m_update_order {0}; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};

class test_reset_order_srv final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_reset_order_srv, service::base);

    //------------------------------------------------------------------------------

    ~test_reset_order_srv() noexcept final = default;

//------------------------------------------------------------------------------

protected:

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*unused*/) final
    {
    }

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
        test_order_srv::s_order = 1;
    }
};

/**
 * @brief Service used to verify that updaters stop before the services they drive.
 */
class test_shutdown_updater final : public app::updater
{
public:

    SIGHT_DECLARE_SERVICE(test_shutdown_updater, app::updater);

    //------------------------------------------------------------------------------

    static void reset()
    {
        std::scoped_lock lock(s_mutex);
        s_stopping = false;
        s_stopped  = false;
        s_release  = false;
    }

    //------------------------------------------------------------------------------

    static bool is_stopping()
    {
        std::scoped_lock lock(s_mutex);
        return s_stopping;
    }

    //------------------------------------------------------------------------------

    static bool is_stopped()
    {
        std::scoped_lock lock(s_mutex);
        return s_stopped;
    }

    //------------------------------------------------------------------------------

    static void release()
    {
        {
            std::scoped_lock lock(s_mutex);
            s_release = true;
        }

        s_condition.notify_all();
    }

protected:

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
        std::unique_lock lock(s_mutex);
        s_stopping = true;
        s_condition.notify_all();
        s_condition.wait(lock, []{return s_release;});
        s_stopped = true;
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
    }

private:

    inline static std::condition_variable s_condition;
    inline static std::mutex s_mutex;
    inline static bool s_stopping {false};
    inline static bool s_stopped {false};
    inline static bool s_release {false};
};

/**
 * @brief Service used to verify that it is stopped after its updater.
 */
class test_shutdown_service final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_shutdown_service, service::base);

    //------------------------------------------------------------------------------

    static void reset()
    {
        std::scoped_lock lock(s_mutex);
        s_stopped_after_updater = false;
        s_stopping              = false;
    }

    //------------------------------------------------------------------------------

    static bool is_stopping()
    {
        std::scoped_lock lock(s_mutex);
        return s_stopping;
    }

    //------------------------------------------------------------------------------

    static bool stopped_after_updater()
    {
        std::scoped_lock lock(s_mutex);
        return s_stopped_after_updater;
    }

protected:

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*unused*/) final
    {
    }

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
        std::scoped_lock lock(s_mutex);
        s_stopping              = true;
        s_stopped_after_updater = test_shutdown_updater::is_stopped();
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
    }

private:

    inline static std::mutex s_mutex;
    inline static bool s_stopping {false};
    inline static bool s_stopped_after_updater {false};
};

/**
 * @brief Service whose update is held until explicitly released.
 */
class test_blocking_update_service final : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_blocking_update_service, service::base);

    //------------------------------------------------------------------------------

    static void reset()
    {
        std::scoped_lock lock(s_mutex);
        s_updating = false;
        s_release  = false;
    }

    //------------------------------------------------------------------------------

    static bool is_updating()
    {
        std::scoped_lock lock(s_mutex);
        return s_updating;
    }

    //------------------------------------------------------------------------------

    static void release()
    {
        {
            std::scoped_lock lock(s_mutex);
            s_release = true;
        }

        s_condition.notify_all();
    }

protected:

    //------------------------------------------------------------------------------

    void starting() final
    {
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
        std::unique_lock lock(s_mutex);
        s_updating = true;
        s_condition.notify_all();
        s_condition.wait(lock, []{return s_release;});
    }

private:

    inline static std::condition_variable s_condition;
    inline static std::mutex s_mutex;
    inline static bool s_updating {false};
    inline static bool s_release {false};
};

} // namespace sight::app::ut
