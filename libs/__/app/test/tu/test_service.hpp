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

#include <core/com/has_slots.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/ptr.hpp>

#include <service/base.hpp>

namespace sight::app::ut
{

/**
 * @brief Service interface for test
 */
class test_service : public service::base
{
public:

    static unsigned int s_START_COUNTER;
    static unsigned int s_UPDATE_COUNTER;
    static const std::string s_OPTION_KEY;
    static const std::string s_UNCONFIGURED;
    static const std::string s_NOT_DEFINED;

    SIGHT_DECLARE_SERVICE(test_service, service::base);
    test_service() noexcept =
        default;

    ~test_service() noexcept override =
        default;

    //------------------------------------------------------------------------------

    void configuring() final
    {
        const config_t cfg = this->get_config();

        m_option = cfg.get(s_OPTION_KEY, s_NOT_DEFINED);
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
    bool getIsUpdated() const
    {
        return m_isUpdated;
    }

    /// return true if the service is updated with update() method
    bool getIsUpdated2() const
    {
        return m_isUpdated2;
    }

    /// return true if the service is updated with update(msg) method
    bool getIsUpdatedMessage() const
    {
        return m_isUpdatedMessage;
    }

    /// return true if the service is updated with update() method
    void resetIsUpdated()
    {
        m_isUpdated = false;
    }

    /// return true if the service is updated with update() method
    void resetIsUpdated2()
    {
        m_isUpdated2 = false;
    }

    //------------------------------------------------------------------------------

    unsigned int getStartOrder() const
    {
        return m_startOrder;
    }

    //------------------------------------------------------------------------------

    unsigned int getUpdateOrder() const
    {
        return m_updateOrder;
    }

    //------------------------------------------------------------------------------

    void setRaiseException(bool raiseException)
    {
        m_raiseException = raiseException;
    }

    //------------------------------------------------------------------------------

    const std::string& getOption() const
    {
        return m_option;
    }

protected:

    bool m_isUpdated {false};
    bool m_isUpdated2 {false};
    bool m_isUpdatedMessage {false};
    bool m_raiseException {false};
    unsigned int m_startOrder {0};
    unsigned int m_updateOrder {0};
    std::string m_option {s_UNCONFIGURED};
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
        using msg_sent_t = core::com::signal<void (const std::string&)>;
    };
    struct slots
    {
        static inline const core::com::slots::key_t UPDATE2 = "update2";
        static inline const core::com::slots::key_t SLOT_1  = "slot1";
    };

    /// Type of signal
    typedef core::com::signal<void (int)> IntSentSignalType;

    //-------------------------------------------------------------------------

    test_srv() noexcept
    {
        new_signal<signals::int_sent_t>(signals::SIG_1);
        new_signal<signals::msg_sent_t>(signals::MSG_SENT);
        new_slot(slots::SLOT_1, &test_srv::receiveSlot, this);
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

    void updating() final
    {
        test_service::updating();
        m_isUpdated = true;
    }

    //-------------------------------------------------------------------------
    void receiveSlot()
    {
        m_received = true;
    }

    //-------------------------------------------------------------------------

    void resetReceive()
    {
        m_received = false;
    }

    //-------------------------------------------------------------------------

    bool getReceived() const
    {
        return m_received;
    }

    //-------------------------------------------------------------------------
    void update2()
    {
        m_isUpdated2 = true;
    }

    //-------------------------------------------------------------------------
    void swapping(std::string_view key) final
    {
        m_swappedObjectKey = key;
        m_swappedObject    = this->input(key).lock().get_shared();
    }

    //-------------------------------------------------------------------------

    const std::string& getSwappedObjectKey() const
    {
        return m_swappedObjectKey;
    }

    //-------------------------------------------------------------------------

    data::object::csptr getSwappedObject() const
    {
        return m_swappedObject;
    }

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data1", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data2", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data3", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data4", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data5", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"dataGroup", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"dataGroup0", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"dataGroup1", data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE}
        };
    }

//-------------------------------------------------------------------------

private:

    bool m_received {false};
    std::string m_swappedObjectKey;
    data::object::csptr m_swappedObject;
};

class STestNoData : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STestNoData, app::ut::test_srv);
};

class STestNoData2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STestNoData2, app::ut::test_srv);
};

class STest1Input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::in> m_input {this, "data1", false};
};

class STest1OptInput : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1OptInput, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::in> m_input {this, "data1", true, true};
};

class STest2Inputs : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest2Inputs, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::in> m_input1 {this, "data1"};
    data::ptr<data::object, data::Access::in> m_input2 {this, "data2"};
};

class STest2InputsV2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest2InputsV2, app::ut::test_srv);

    //-------------------------------------------------------------------------

    connections_t auto_connections() const override
    {
        return {
            {"data1", data::object::MODIFIED_SIG, service::slots::UPDATE},
            {"data2", data::object::MODIFIED_SIG, slots::SLOT_1},
        };
    }

private:

    data::ptr<data::object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::object, data::Access::in> m_input2 {this, "data2", true};
};

class STest4Inputs : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest4Inputs, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::in> m_inout1 {this, "data1", true};
    data::ptr<data::object, data::Access::in> m_inout2 {this, "data2", true, true};
    data::ptr<data::object, data::Access::in> m_inout3 {this, "data3", true, true};
    data::ptr<data::object, data::Access::in> m_inout4 {this, "data4", true, true};
};

class STest1Inout : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1Inout, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::inout> m_inout {this, "data1", true};
};

class STest3Inouts : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest3Inouts, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::object, data::Access::inout> m_inout3 {this, "data3", true};
};

class STest3InoutsV2 : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest3InoutsV2, app::ut::test_srv);

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

    data::ptr<data::object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::object, data::Access::inout> m_inout3 {this, "data3", false};
};

class STest2Inouts1Input : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest2Inouts1Input, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::object, data::Access::in> m_input3 {this, "data3", true};
};

class STest1Input1OptInput1OptInOut : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1OptInput1OptInOut, app::ut::test_srv);

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

    data::ptr<data::object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::object, data::Access::in> m_input2 {this, "data2", true, true};
    data::ptr<data::object, data::Access::inout> m_inout {this, "data3", false, true};
};

class STest2InputGroups : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest2InputGroups, app::ut::test_srv);

    data::ptr_vector<data::object, data::Access::in> m_input1 {this, "dataGroup0", false};
    data::ptr_vector<data::object, data::Access::in> m_input2 {this, "dataGroup1", false};
};

class STest1Input1InputGroup : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1InputGroup, app::ut::test_srv);

    data::ptr<data::object, data::Access::in> m_input {this, "data1", true, true};
    data::ptr_vector<data::object, data::Access::in> m_inputGroup {this, "dataGroup", true};
};

class STest1Input1Output2Inouts : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1Output2Inouts, app::ut::test_srv);

private:

    data::ptr<data::object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::object, data::Access::out> m_out2 {this, "data2", true};
    data::ptr<data::object, data::Access::inout> m_inout3 {this, "data3", true};
    data::ptr<data::object, data::Access::inout> m_inout4 {this, "data4", true};
};

class STestOut : public test_srv
{
public:

    SIGHT_DECLARE_SERVICE(STestOut, app::ut::test_srv);

    data::ptr<data::object, data::Access::out> m_out {this, "out"};
    data::ptr<data::object, data::Access::out> m_out2 {this, "out2"};
    data::ptr<data::object, data::Access::out> m_out3 {this, "out3"};
    data::ptr<data::object, data::Access::out> m_out4 {this, "out4"};
    data::ptr<data::object, data::Access::out> m_out5 {this, "out5"};
    data::ptr_vector<data::object, data::Access::out> m_outGroup {this, "outGroup"};
};

class test_service_with_data : public service::base
{
public:

    static const key_t s_INPUT;
    static const key_t s_INOUT_GROUP;
    static const key_t s_OUTPUT;

    SIGHT_DECLARE_SERVICE(test_service_with_data, service::base);
    test_service_with_data() noexcept           = default;
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

    data::ptr<data::object, data::Access::in> m_input {this, "input", true};
    data::ptr_vector<data::integer, data::Access::inout> m_inoutGroup {this, "inoutGroup", true};
    data::ptr<data::object, data::Access::out> m_output {this, "output", false, true};
};

} // namespace sight::app::ut
