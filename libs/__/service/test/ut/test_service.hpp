/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/com/has_slots.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/ptr.hpp>

#include <service/base.hpp>

namespace sight::service::ut
{

/**
 * @brief Service interface for test
 */
class test_service : public service::base
{
public:

    static unsigned int s_start_counter;
    static unsigned int s_update_counter;
    static const std::string OPTION_KEY;
    static const std::string UNCONFIGURED;
    static const std::string NOT_DEFINED;

    SIGHT_DECLARE_SERVICE(test_service, service::base);

    test_service() noexcept = default;
    ~test_service() noexcept override = default;

    //------------------------------------------------------------------------------

    void configuring() final
    {
        const config_t cfg = this->get_config();

        m_option = cfg.get(OPTION_KEY, NOT_DEFINED);
    }

    void starting() override;
    //------------------------------------------------------------------------------

    void stopping() final;
    void updating() override;
    //------------------------------------------------------------------------------

    void info(std::ostream& _sstream) override
    {
        _sstream << "test_service";
    }

    /// return true if the service is updated with update() method
    bool is_updated() const
    {
        return m_is_updated;
    }

    /// return true if the service is updated with update() method
    bool is_updated2() const
    {
        return m_is_updated2;
    }

    /// return true if the service is updated with update(msg) method
    bool is_updated_message() const
    {
        return m_is_updated_message;
    }

    /// return true if the service is updated with update() method
    void reset_is_updated()
    {
        m_is_updated = false;
    }

    /// return true if the service is updated with update() method
    void reset_is_updated2()
    {
        m_is_updated2 = false;
    }

    //------------------------------------------------------------------------------

    unsigned int get_start_order() const
    {
        return m_start_order;
    }

    //------------------------------------------------------------------------------

    unsigned int get_update_order() const
    {
        return m_update_order;
    }

    //------------------------------------------------------------------------------

    void set_raise_exception(bool _raise_exception)
    {
        m_raise_exception = _raise_exception;
    }

    //------------------------------------------------------------------------------

    const std::string& get_option() const
    {
        return m_option;
    }

protected:

    bool m_is_updated {false};
    bool m_is_updated2 {false};
    bool m_is_updated_message {false};
    bool m_raise_exception {false};
    unsigned int m_start_order {0};
    unsigned int m_update_order {0};
    std::string m_option {UNCONFIGURED};
};

/**
 * @brief Service interface for autoconnect test
 */
class test_srv : public test_service
{
public:

    SIGHT_DECLARE_CLASS(test_srv, test_service);

    struct signals
    {
        static inline const core::com::signals::key_t SIG_1    = "signal1";
        static inline const core::com::signals::key_t MSG_SENT = "msgSent";

        using int_sent_t = core::com::signal<void (int)>;
        using msg_sent_t = core::com::signal<void (std::string)>;
    };
    struct slots
    {
        static inline const core::com::slots::key_t UPDATE2 = "update2";
        static inline const core::com::slots::key_t SLOT_1  = "slot1";
    };

    /// Type of signal
    using int_sent_signal_t = core::com::signal<void (int)>;

    //-------------------------------------------------------------------------

    test_srv() noexcept
    {
        new_signal<signals::int_sent_t>(signals::SIG_1);
        new_signal<signals::msg_sent_t>(signals::MSG_SENT);
        new_slot(slots::SLOT_1, &test_srv::receive_slot, this);
        new_slot(slots::UPDATE2, &test_srv::update2, this);
    }

    //-------------------------------------------------------------------------
    ~test_srv() noexcept override =
        default;

    //-------------------------------------------------------------------------
    void starting() final
    {
        test_service::starting();
    }

    //-------------------------------------------------------------------------

    void updating() override
    {
        test_service::updating();
        m_is_updated = true;
    }

    //-------------------------------------------------------------------------
    void receive_slot()
    {
        m_received = true;
    }

    //-------------------------------------------------------------------------

    void reset_receive()
    {
        m_received = false;
    }

    //-------------------------------------------------------------------------

    bool get_received() const
    {
        return m_received;
    }

    //-------------------------------------------------------------------------
    void update2()
    {
        m_is_updated2 = true;
    }

    //-------------------------------------------------------------------------
    void swapping(std::string_view _key) final
    {
        m_swapped_object_key = _key;
        m_swapped_object     = this->input(_key).lock().get_shared();
    }

    //-------------------------------------------------------------------------

    const std::string& get_swapped_object_key() const
    {
        return m_swapped_object_key;
    }

    //-------------------------------------------------------------------------

    data::object::csptr get_swapped_object() const
    {
        return m_swapped_object;
    }

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data1", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data2", data::object::MODIFIED_SIG, slots::UPDATE2}
        };
    }

//-------------------------------------------------------------------------

private:

    bool m_received {false};
    std::string m_swapped_object_key;
    data::object::csptr m_swapped_object;
};

class test_no_data : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_no_data, service::ut::test_srv);
};

class test_no_data2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_no_data2, service::ut::test_srv);
};

class test1_input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input, service::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_input {this, "data1"};
};

class test1_inout : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_inout, service::ut::test_srv);

private:

    data::ptr<data::object, data::access::inout> m_inout {this, "data1"};
};

class test2_inouts1_input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test2_inouts1_input, service::ut::test_srv);

private:

    data::ptr<data::object, data::access::inout> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::inout> m_inout2 {this, "data2"};
    data::ptr<data::object, data::access::in> m_input3 {this, "data3"};
};

class test1_input1_opt_input1_opt_in_out : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input1_opt_input1_opt_in_out, service::ut::test_srv);

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        connections_t connections;
        connections.push("data1", data::object::MODIFIED_SIG, service::slots::UPDATE);
        connections.push("data2", data::object::MODIFIED_SIG, slots::SLOT_1);
        connections.push("data3", data::object::MODIFIED_SIG, slots::SLOT_1);
        return connections;
    }

private:

    data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::access::in> m_input2 {this, "data2", true};
    data::ptr<data::object, data::access::inout> m_inout {this, "data3", true};
};

class test1_property : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_property, service::ut::test_srv);

    data::property<data::integer> m_prop1 {this, "prop1", 12};
};

class test_service_with_data : public service::base
{
public:

    static inline const key_t INPUT       = "input";
    static inline const key_t INOUT_GROUP = "inoutGroup";
    static inline const key_t OUTPUT      = "output";

    SIGHT_DECLARE_SERVICE(test_service_with_data, service::base);
    test_service_with_data() noexcept = default;
    ~test_service_with_data() noexcept override = default;

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override;
    void updating() override;

    data::ptr<data::object, data::access::in> m_input {this, INPUT};
    data::ptr_vector<data::integer, data::access::inout> m_inout_group {this, INOUT_GROUP};
    data::ptr<data::object, data::access::out> m_output {this, OUTPUT, true};
};

class test3_inouts_v2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test3_inouts_v2, service::ut::test_srv);

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data1", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data2", data::object::MODIFIED_SIG, slots::SLOT_1},
            {"data3", data::object::MODIFIED_SIG, slots::SLOT_1},
        };
    }

private:

    data::ptr<data::object, data::access::inout> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::inout> m_inout2 {this, "data2"};
    data::ptr<data::object, data::access::inout> m_inout3 {this, "data3"};
};

} // namespace sight::service::ut
