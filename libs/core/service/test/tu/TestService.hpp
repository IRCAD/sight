/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::service
{

namespace ut
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
    TestService() noexcept
    {
    }

    virtual ~TestService() noexcept
    {
    }

    //------------------------------------------------------------------------------

    virtual void configuring() final
    {
        const ConfigType cfg = this->getConfigTree();

        m_option = cfg.get(s_OPTION_KEY, s_NOT_DEFINED);
    }

    void starting() override;
    //------------------------------------------------------------------------------

    virtual void stopping() final;
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
 * @brief Service implementation for test
 */
class TestServiceImplementation : public TestService
{
public:

    SIGHT_DECLARE_SERVICE(TestServiceImplementation, service::ut::TestService);

    /// Keys to register Signal
    static const core::com::Signals::SignalKeyType s_MSG_SENT_SIG;
    /// Keys to register Slot
    static const core::com::Slots::SlotKeyType s_UPDATE2_SLOT;

    /// Type os signal
    typedef core::com::Signal<void (const std::string&)> MsgSentSignalType;

    //-------------------------------------------------------------------------
    TestServiceImplementation() noexcept;

    //-------------------------------------------------------------------------
    virtual ~TestServiceImplementation() noexcept;

    //-------------------------------------------------------------------------
    virtual void starting() final
    {
        TestService::starting();
    }

    //-------------------------------------------------------------------------
    void updating() override
    {
        TestService::updating();
        m_isUpdated = true;
    }

    //-------------------------------------------------------------------------
    void update2()
    {
        m_isUpdated2 = true;
    }

    //-------------------------------------------------------------------------
    void swapping(const KeyType& key) override
    {
        m_swappedObjectKey = key;
        m_swappedObject    = this->getInput<data::Object>(key);
    }

    //-------------------------------------------------------------------------
    KeyConnectionsMap getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data1", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data2", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data3", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data4", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data5", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("dataGroup", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        return connections;
    }

    //-------------------------------------------------------------------------
    void info(std::ostream& _sstream) override
    {
        _sstream << "TestServiceImplementation";
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

private:

    std::string m_swappedObjectKey;
    data::Object::csptr m_swappedObject;
};

/**
 * @brief Service implementation for test
 */
class TestServiceImplementation2 : public TestService
{
public:

    SIGHT_DECLARE_SERVICE(TestServiceImplementation2, service::ut::TestService);

    //-------------------------------------------------------------------------
    TestServiceImplementation2() noexcept
    {
    }

    //-------------------------------------------------------------------------
    virtual ~TestServiceImplementation2() noexcept
    {
    }
};

/**
 * @brief Service implementation for autoconnect test
 */
class TestSrvAutoconnect : public TestService
{
public:

    SIGHT_DECLARE_SERVICE(TestSrvAutoconnect, service::ut::TestService);

    /// Keys to register Signal
    static const core::com::Signals::SignalKeyType s_SIG_1;
    /// Keys to register Slot
    static const core::com::Slots::SlotKeyType s_SLOT_1;

    /// Type os signal
    typedef core::com::Signal<void (int)> MsgSentSignalType;

    //-------------------------------------------------------------------------

    TestSrvAutoconnect() noexcept :
        m_received(false)
    {
        newSignal<MsgSentSignalType>(s_SIG_1);
        newSlot(s_SLOT_1, &TestSrvAutoconnect::receiveSlot, this);
    }

    //-------------------------------------------------------------------------
    virtual ~TestSrvAutoconnect() noexcept
    {
    }

    //-------------------------------------------------------------------------

    void updating() override
    {
        m_isUpdated = true;
    }

    //-------------------------------------------------------------------------

    void info(std::ostream& _sstream) override
    {
        _sstream << "TestSrvAutoconnect";
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

    IService::KeyConnectionsMap getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data1", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data2", data::Object::s_MODIFIED_SIG, s_SLOT_1);
        connections.push("data3", data::Object::s_MODIFIED_SIG, s_SLOT_1);
        connections.push("dataGroup0", data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("dataGroup1", data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

        return connections;
    }

//-------------------------------------------------------------------------

private:

    int m_received;
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

    data::ptr<data::Object, data::Access::in> m_input {this, "input"};
    data::ptr_vector<data::Integer, data::Access::inout> m_inoutGroup {this, "inoutGroup", true, 2};
    data::ptr<data::Object, data::Access::out> m_output {this, "output", false, true};
};

} //namespace ut

} //namespace sight::service
