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

#include <core/com/has_slots.hpp>
#include <core/com/signal.hpp>

#include <data/boolean.hpp>
#include <data/dvec3.hpp>
#include <data/generic.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/ptr.hpp>
#include <data/string.hpp>

#include <service/base.hpp>

namespace sight::app::ut
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

    /// return true if the service is updated with update() method
    bool is_updated() const
    {
        return m_is_updated;
    }

    /// return true if the update2() slot has been called
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

    //------------------------------------------------------------------------------

    void configuring() override
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

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    bool m_is_updated {false};
    bool m_is_updated2 {false};
    bool m_is_updated_message {false};
    bool m_raise_exception {false};
    unsigned int m_start_order {0};
    unsigned int m_update_order {0};
    std::string m_option {UNCONFIGURED};
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
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
        static inline const signal_key_t SIG_1    = "signal1";
        static inline const signal_key_t MSG_SENT = "msgSent";

        using int_sent_t = core::com::signal<void (int)>;
        using msg_sent_t = core::com::signal<void (const std::string&)>;
    };
    struct slots
    {
        static inline const slot_key_t UPDATE2 = "update2";
        static inline const slot_key_t SLOT_1  = "slot1";
    };

    /// Type of signal
    using int_sent_t = core::com::signal<void (int)>;

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

    const std::string& get_swapped_object_key() const
    {
        return m_swapped_object_key;
    }

    //-------------------------------------------------------------------------

    data::object::csptr get_swapped_object() const
    {
        return m_swapped_object;
    }

protected:

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data", data::signals::MODIFIED, service::slots::UPDATE},
            {"data1", data::signals::MODIFIED, service::slots::UPDATE},
            {"data2", data::signals::MODIFIED, service::slots::UPDATE},
            {"data3", data::signals::MODIFIED, service::slots::UPDATE},
            {"data4", data::signals::MODIFIED, service::slots::UPDATE},
            {"data5", data::signals::MODIFIED, service::slots::UPDATE},
            {"dataGroup", data::signals::MODIFIED, service::slots::UPDATE},
            {"dataGroup0", data::signals::MODIFIED, service::slots::UPDATE},
            {"dataGroup1", data::image::signals::BUFFER_MODIFIED, service::slots::UPDATE}
        };
    }

    //-------------------------------------------------------------------------
    void starting() final
    {
        test_service::starting();
    }

    //-------------------------------------------------------------------------

    void updating() final
    {
        test_service::updating();
        m_is_updated = true;
    }

    //-------------------------------------------------------------------------
    void swapping(std::string_view _key) final
    {
        m_swapped_object_key = _key;
        m_swapped_object     = this->input(_key).lock().get_shared();
    }

//-------------------------------------------------------------------------

private:

    bool m_received {false};
    std::string m_swapped_object_key;
    data::object::csptr m_swapped_object;
};

class test_config : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_config, app::ut::test_srv);

    sight::service::base::config_t export_config() const;

protected:

    void configuring() override;

private:

    mutable std::recursive_mutex m_config_mutex;
    sight::service::base::config_t m_config;
};

class test_no_data : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_no_data, app::ut::test_srv);
};

class test_no_data2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_no_data2, app::ut::test_srv);
};

class test1_input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_input {this, "data1"};
};

class test1_opt_input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_opt_input, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_input {this, "data1", true};
};

class test2_inputs : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test2_inputs, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::access::in> m_input2 {this, "data2"};
};

class test2_inputs_v2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test2_inputs_v2, app::ut::test_srv);

protected:

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data1", data::signals::MODIFIED, service::slots::UPDATE},
            {"data2", data::signals::MODIFIED, slots::SLOT_1},
        };
    }

private:

    data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::access::in> m_input2 {this, "data2"};
};

class test4_inputs : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test4_inputs, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::in> m_inout2 {this, "data2", true};
    data::ptr<data::object, data::access::in> m_inout3 {this, "data3", true};
    data::ptr<data::object, data::access::in> m_inout4 {this, "data4", true};
};

class test1_inout : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_inout, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::inout> m_inout {this, "data1"};
};

class test3_inouts : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test3_inouts, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::inout> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::inout> m_inout2 {this, "data2"};
    data::ptr<data::object, data::access::inout> m_inout3 {this, "data3"};
};

class test3_inouts_v2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test3_inouts_v2, app::ut::test_srv);

protected:

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data1", data::signals::MODIFIED, service::slots::UPDATE},
            {"data2", data::signals::MODIFIED, slots::SLOT_1},
            {"data3", data::signals::MODIFIED, slots::SLOT_1},
        };
    }

private:

    data::ptr<data::object, data::access::inout> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::inout> m_inout2 {this, "data2"};
    data::ptr<data::object, data::access::inout> m_inout3 {this, "data3"};
};

class test2_inouts1_input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test2_inouts1_input, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::inout> m_inout1 {this, "data1"};
    data::ptr<data::object, data::access::inout> m_inout2 {this, "data2"};
    data::ptr<data::object, data::access::in> m_input3 {this, "data3"};
};

class test1_input1_opt_input1_opt_in_out : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input1_opt_input1_opt_in_out, app::ut::test_srv);

protected:

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        connections_t connections;
        connections.push("data1", data::signals::MODIFIED, service::slots::UPDATE);
        connections.push("data2", data::signals::MODIFIED, slots::SLOT_1);
        connections.push("data3", data::signals::MODIFIED, slots::SLOT_1);
        return connections;
    }

private:

    data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::access::in> m_input2 {this, "data2", true};
    data::ptr<data::object, data::access::inout> m_inout {this, "data3", true};
};

class test2_input_groups : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test2_input_groups, app::ut::test_srv);

    data::ptr_vector<data::object, data::access::in> m_input1 {this, "dataGroup0"};
    data::ptr_vector<data::object, data::access::in> m_input2 {this, "dataGroup1"};
};

class test1_input1_input_group : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input1_input_group, app::ut::test_srv);

    data::ptr<data::object, data::access::in> m_input {this, "data1", true};
    data::ptr_vector<data::object, data::access::in> m_input_group {this, "dataGroup"};
};

class test1_input1_output2_inouts : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input1_output2_inouts, app::ut::test_srv);

private:

    data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::access::out> m_out2 {this, "data2"};
    data::ptr<data::object, data::access::inout> m_inout3 {this, "data3"};
    data::ptr<data::object, data::access::inout> m_inout4 {this, "data4"};
};

class test1_input1_property : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test1_input1_property, app::ut::test_srv);

private:

    sight::data::ptr<data::object, data::access::in> m_input1 {this, "data1"};
    sight::data::ptr<data::integer> m_int_property1 {this, "int_property1"};
};

class test_out : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(test_out, app::ut::test_srv);

    data::ptr<data::object, data::access::out> m_out {this, "out"};
    data::ptr<data::object, data::access::out> m_out2 {this, "out2"};
    data::ptr<data::object, data::access::out> m_out3 {this, "out3"};
    data::ptr<data::object, data::access::out> m_out4 {this, "out4"};
    data::ptr<data::object, data::access::out> m_out5 {this, "out5"};
    data::ptr_vector<data::object, data::access::out> m_out_group {this, "outGroup"};
};

class test_service_with_data : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_service_with_data, service::base);
    test_service_with_data() noexcept = default;
    ~test_service_with_data() noexcept override = default;

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

    void stopping() override;
    void updating() override;

public:

    data::ptr<data::object, data::access::in> m_input {this, "input"};
    data::ptr_vector<data::integer, data::access::inout> m_inout_group {this, "inoutGroup"};
    data::ptr<data::object, data::access::out> m_output {this, "output", true};
};

class test_service_with_typed_data : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_service_with_typed_data, service::base);
    test_service_with_typed_data() noexcept = default;
    ~test_service_with_typed_data() noexcept override = default;

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

public:

    data::ptr<data::boolean, data::access::in> m_flag {this, "flag"};
    data::ptr<data::dvec3, data::access::inout> m_position {this, "position"};

    /// Declared with a default value, so it can be omitted in the configuration.
    data::ptr<data::integer, data::access::in> m_threshold {this, "threshold", 50};
    data::ptr<data::integer, data::access::inout> m_offset {this, "offset", -3};
};

class test_service_with_properties : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_service_with_properties, service::base);
    test_service_with_properties() noexcept = default;
    ~test_service_with_properties() noexcept override = default;

    //------------------------------------------------------------------------------

    void set_integer(std::int64_t _value)
    {
        auto int_prop = m_int_prop.lock();
        *int_prop     = _value;

        int_prop->async_emit(this, data::signals::MODIFIED);
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
        m_slot_called = true;
    }

    //------------------------------------------------------------------------------

    void on_property_set(std::string_view _key) override
    {
        m_callback_called_parameter = _key;
        if(_key == "integer")
        {
            if(m_emit_signal)
            {
                set_integer(*m_int_prop);
                ++m_signal_count;
            }
        }
    }

    //------------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {m_vec_prop, data::signals::MODIFIED, service::slots::UPDATE}
        };
    }

public:

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    data::property<data::integer> m_int_prop {this, "integer", 42};
    data::property<data::string> m_string_prop {this, "string", {"default_value"}};
    data::property<data::dvec3> m_vec_prop {this, "vec", {12.123, 56.0, 78.56}};
    bool m_slot_called {false};
    bool m_emit_signal {false};
    std::size_t m_signal_count {0U};
    std::string_view m_callback_called_parameter {};
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

/// Declares hierarchical keys, so that the XML configuration can mirror their structure.
class test_service_with_nested_keys : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(test_service_with_nested_keys, service::base);
    test_service_with_nested_keys() noexcept = default;
    ~test_service_with_nested_keys() noexcept override = default;

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

public:

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    data::ptr<data::object, data::access::in> m_source {this, "image.source"};
    data::ptr<data::object, data::access::inout> m_target {this, "image.target", true};
    data::property<data::integer> m_threshold {this, "config.threshold", 5};
    data::property<data::string> m_label {this, "config.label", {"default_label"}};
    data::ptr_vector<data::string, data::access::in> m_tracker_ip {this, "config.tracker.ip"};
    data::ptr_vector<data::integer, data::access::in> m_tracker_port {this, "config.tracker.port"};
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

} // namespace sight::app::ut
