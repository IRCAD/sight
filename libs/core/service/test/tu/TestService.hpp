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

#include <core/com/HasSlots.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/ptr.hpp>

#include <service/IService.hpp>

namespace sight::service::ut
{

/**
 * @brief Service interface for test
 */
class TestService : public service::IService
{
public:

    static unsigned int s_START_COUNTER;
    static unsigned int s_UPDATE_COUNTER;
    static const std::string s_OPTION_KEY;
    static const std::string s_UNCONFIGURED;
    static const std::string s_NOT_DEFINED;

    SIGHT_DECLARE_SERVICE(TestService, service::IService);
    TestService() noexcept =
        default;

    ~TestService() noexcept override =
        default;

    //------------------------------------------------------------------------------

    void configuring() final
    {
        const ConfigType cfg = this->getConfiguration();

        m_option = cfg.get(s_OPTION_KEY, s_NOT_DEFINED);
    }

    void starting() override;
    //------------------------------------------------------------------------------

    void stopping() final;
    void updating() override;
    //------------------------------------------------------------------------------

    void info(std::ostream& _sstream) override
    {
        _sstream << "TestService";
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
class ISTest : public TestService
{
public:

    SIGHT_DECLARE_CLASS(ISTest, TestService);

    /// Keys to register Signal
    static const core::com::Signals::SignalKeyType s_MSG_SENT_SIG;
    /// Keys to register Slot
    static const core::com::Slots::SlotKeyType s_UPDATE2_SLOT;

    /// Type of signal
    typedef core::com::Signal<void (const std::string&)> MsgSentSignalType;

    /// Keys to register Signal
    static const core::com::Signals::SignalKeyType s_SIG_1;
    /// Keys to register Slot
    static const core::com::Slots::SlotKeyType s_SLOT_1;

    /// Type of signal
    typedef core::com::Signal<void (int)> IntSentSignalType;

    //-------------------------------------------------------------------------

    ISTest() noexcept
    {
        newSignal<IntSentSignalType>(s_SIG_1);
        newSignal<MsgSentSignalType>(s_MSG_SENT_SIG);
        newSlot(s_SLOT_1, &ISTest::receiveSlot, this);
        newSlot(s_UPDATE2_SLOT, &ISTest::update2, this);
    }

    //-------------------------------------------------------------------------
    ~ISTest() noexcept override =
        default;

    //-------------------------------------------------------------------------
    void starting() final
    {
        TestService::starting();
    }

    //-------------------------------------------------------------------------

    void updating() final
    {
        TestService::updating();
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
        m_swappedObject    = this->getInput(key).lock().get_shared();
    }

    //-------------------------------------------------------------------------

    const std::string& getSwappedObjectKey() const
    {
        return m_swappedObjectKey;
    }

    //-------------------------------------------------------------------------

    data::Object::csptr getSwappedObject() const
    {
        return m_swappedObject;
    }

    //-------------------------------------------------------------------------

    IService::KeyConnectionsMap getAutoConnections() const override
    {
        return {
            {"data", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data1", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data2", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data3", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data4", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data5", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"dataGroup", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"dataGroup0", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"dataGroup1", data::Image::s_BUFFER_MODIFIED_SIG, slots::s_UPDATE}
        };
    }

//-------------------------------------------------------------------------

private:

    bool m_received {false};
    std::string m_swappedObjectKey;
    data::Object::csptr m_swappedObject;
};

class STestNoData : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STestNoData, service::ut::ISTest);
};

class STestNoData2 : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STestNoData2, service::ut::ISTest);
};

class STest1Input : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::in> m_input {this, "data1", false};
};

class STest1OptInput : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1OptInput, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::in> m_input {this, "data1", true, true};
};

class STest2Inputs : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest2Inputs, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::in> m_input1 {this, "data1"};
    data::ptr<data::Object, data::Access::in> m_input2 {this, "data2"};
};

class STest2InputsV2 : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest2InputsV2, service::ut::ISTest);

    //-------------------------------------------------------------------------

    IService::KeyConnectionsMap getAutoConnections() const override
    {
        return {
            {"data1", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data2", data::Object::s_MODIFIED_SIG, s_SLOT_1},
        };
    }

private:

    data::ptr<data::Object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::Object, data::Access::in> m_input2 {this, "data2", true};
};

class STest4Inputs : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest4Inputs, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::in> m_inout1 {this, "data1", true};
    data::ptr<data::Object, data::Access::in> m_inout2 {this, "data2", true, true};
    data::ptr<data::Object, data::Access::in> m_inout3 {this, "data3", true, true};
    data::ptr<data::Object, data::Access::in> m_inout4 {this, "data4", true, true};
};

class STest1Inout : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1Inout, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::inout> m_inout {this, "data1", true};
};

class STest3Inouts : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest3Inouts, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::Object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::Object, data::Access::inout> m_inout3 {this, "data3", true};
};

class STest3InoutsV2 : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest3InoutsV2, service::ut::ISTest);

    //-------------------------------------------------------------------------

    IService::KeyConnectionsMap getAutoConnections() const override
    {
        return {
            {"data1", data::Object::s_MODIFIED_SIG, slots::s_UPDATE},
            {"data2", data::Object::s_MODIFIED_SIG, s_SLOT_1},
            {"data3", data::Object::s_MODIFIED_SIG, s_SLOT_1},
        };
    }

private:

    data::ptr<data::Object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::Object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::Object, data::Access::inout> m_inout3 {this, "data3", false};
};

class STest2Inouts1Input : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest2Inouts1Input, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::inout> m_inout1 {this, "data1", true};
    data::ptr<data::Object, data::Access::inout> m_inout2 {this, "data2", true};
    data::ptr<data::Object, data::Access::in> m_input3 {this, "data3", true};
};

class STest1Input1OptInput1OptInOut : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1OptInput1OptInOut, service::ut::ISTest);

    //-------------------------------------------------------------------------

    IService::KeyConnectionsMap getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data1", data::Object::s_MODIFIED_SIG, slots::s_UPDATE);
        connections.push("data2", data::Object::s_MODIFIED_SIG, s_SLOT_1);
        connections.push("data3", data::Object::s_MODIFIED_SIG, s_SLOT_1);
        return connections;
    }

private:

    data::ptr<data::Object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::Object, data::Access::in> m_input2 {this, "data2", true, true};
    data::ptr<data::Object, data::Access::inout> m_inout {this, "data3", false, true};
};

class STest2InputGroups : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest2InputGroups, service::ut::ISTest);

    data::ptr_vector<data::Object, data::Access::in> m_input1 {this, "dataGroup0", false};
    data::ptr_vector<data::Object, data::Access::in> m_input2 {this, "dataGroup1", false};
};

class STest1Input1InputGroup : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1InputGroup, service::ut::ISTest);

    data::ptr<data::Object, data::Access::in> m_input {this, "data1", true, true};
    data::ptr_vector<data::Object, data::Access::in> m_inputGroup {this, "dataGroup", true};
};

class STest1Input1Output2Inouts : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STest1Input1Output2Inouts, service::ut::ISTest);

private:

    data::ptr<data::Object, data::Access::in> m_input1 {this, "data1", true};
    data::ptr<data::Object, data::Access::out> m_out2 {this, "data2", true};
    data::ptr<data::Object, data::Access::inout> m_inout3 {this, "data3", true};
    data::ptr<data::Object, data::Access::inout> m_inout4 {this, "data4", true};
};

class STestOut : public ISTest
{
public:

    SIGHT_DECLARE_SERVICE(STestOut, service::ut::ISTest);

    data::ptr<data::Object, data::Access::out> m_out {this, "out"};
    data::ptr<data::Object, data::Access::out> m_out2 {this, "out2"};
    data::ptr<data::Object, data::Access::out> m_out3 {this, "out3"};
    data::ptr<data::Object, data::Access::out> m_out4 {this, "out4"};
    data::ptr<data::Object, data::Access::out> m_out5 {this, "out5"};
    data::ptr_vector<data::Object, data::Access::out> m_outGroup {this, "outGroup"};
};

class TestServiceWithData : public service::IService
{
public:

    static const KeyType s_INPUT;
    static const KeyType s_INOUT_GROUP;
    static const KeyType s_OUTPUT;

    SIGHT_DECLARE_SERVICE(TestServiceWithData, service::IService);
    TestServiceWithData() noexcept           = default;
    ~TestServiceWithData() noexcept override = default;

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

    data::ptr<data::Object, data::Access::in> m_input {this, "input", true};
    data::ptr_vector<data::Integer, data::Access::inout> m_inoutGroup {this, "inoutGroup", true};
    data::ptr<data::Object, data::Access::out> m_output {this, "output", false, true};
};

} // namespace sight::service::ut
