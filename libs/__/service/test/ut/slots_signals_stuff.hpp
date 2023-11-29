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

#include <core/com/signals.hpp>

#include <service/base.hpp>

namespace sight::service::ut
{

//------------------------------------------------------------------------------

class service_retarder
{
public:

    service_retarder() :
        m_start_retarder(500),
        m_stop_retarder(500),
        m_update_retarder(500),
        m_receive_retarder(500),
        m_swap_retarder(500)
    {
    }

    std::chrono::milliseconds m_start_retarder;
    std::chrono::milliseconds m_stop_retarder;
    std::chrono::milliseconds m_update_retarder;
    std::chrono::milliseconds m_receive_retarder;
    std::chrono::milliseconds m_swap_retarder;
};

//------------------------------------------------------------------------------

class buffer : public data::object
{
public:

    SIGHT_DECLARE_CLASS(buffer, data::object);

    //------------------------------------------------------------------------------

    void shallow_copy(const object::csptr& /*unused*/) override
    {
    }
};

//------------------------------------------------------------------------------

class basic_srv : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(basic_srv, service::base);

    static constexpr std::string_view BUFFER_INOUT = "buffer";

protected:

    data::ptr<buffer, data::access::inout> m_buffer {this, BUFFER_INOUT, true};
};

class s_basic_test : public basic_srv,
                     public service_retarder
{
public:

    SIGHT_DECLARE_SERVICE(s_basic_test, basic_srv);

    bool m_update_finished {false};
    bool m_swap_finished {false};

    s_basic_test();

protected:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    void starting() override;
    void stopping() override;
    void swapping(std::string_view _key) override;
    void updating() override;
};

//------------------------------------------------------------------------------

class reader_test : public basic_srv
{
public:

    SIGHT_DECLARE_SERVICE(reader_test, basic_srv);

    reader_test()
    = default;

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

    void updating() override;
};

//------------------------------------------------------------------------------

class reader2_test : public basic_srv
{
public:

    using changed_signal_t = core::com::signal<void ()>;

    SIGHT_DECLARE_SERVICE(reader2_test, basic_srv);

    reader2_test();

    static const core::com::signals::key_t CHANGED_SIG;

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

    void updating() override;

    changed_signal_t::sptr m_sig_changed;
};

//------------------------------------------------------------------------------

class s_show_test : public basic_srv,
                    public service_retarder
{
public:

    SIGHT_DECLARE_SERVICE(s_show_test, basic_srv);

    s_show_test();

    int m_receive_count {0};
    int m_change_count {0};

    void change();

    static const core::com::slots::key_t CHANGE_SLOT;
    using change_slot_t = core::com::slot<void ()>;

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

    void updating() override;

    connections_t auto_connections() const override;

    change_slot_t::sptr m_slot_change;

    core::mt::mutex m_mutex;
};

//------------------------------------------------------------------------------

class s_show2_test : public basic_srv,
                     public service_retarder
{
public:

    SIGHT_DECLARE_SERVICE(s_show2_test, basic_srv);

    static const core::com::slots::key_t UPDATE_BUFFER_SLOT;

    s_show2_test();

    int m_receive_count {0};

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

    void updating() override;

    /// Slot to receive update
    void update_buffer();
};

//------------------------------------------------------------------------------

} // namespace sight::service::ut
