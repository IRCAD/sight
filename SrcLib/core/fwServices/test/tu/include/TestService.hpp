/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwServices/IService.hpp>

#include <fwCom/HasSlots.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief Service interface for test
 */
class TestService : public ::fwServices::IService
{

public:

    static unsigned int s_START_COUNTER;
    static unsigned int s_UPDATE_COUNTER;
    static const std::string s_OPTION_KEY;
    static const std::string s_UNCONFIGURED;
    static const std::string s_NOT_DEFINED;

    fwCoreServiceClassDefinitionsMacro( (TestService)(::fwServices::IService) );
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
    virtual void starting() override;
    //------------------------------------------------------------------------------

    virtual void stopping() final;
    virtual void updating() override;
    //------------------------------------------------------------------------------

    virtual void info(std::ostream& _sstream ) override
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
    bool m_isUpdated { false };
    bool m_isUpdated2 { false };
    bool m_isUpdatedMessage { false };
    bool m_raiseException { false };
    unsigned int m_startOrder { 0 };
    unsigned int m_updateOrder { 0 };
    std::string m_option{s_UNCONFIGURED};
};

/**
 * @brief Service implementation for test
 */
class TestServiceImplementation : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro( (TestServiceImplementation)(::fwServices::ut::TestService) );

    /// Keys to register Signal
    static const ::fwCom::Signals::SignalKeyType s_MSG_SENT_SIG;
    /// Keys to register Slot
    static const ::fwCom::Slots::SlotKeyType s_UPDATE2_SLOT;

    /// Type os signal
    typedef ::fwCom::Signal< void (const std::string&)> MsgSentSignalType;

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
    virtual void updating() override
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
    virtual void swapping(const KeyType& key) override
    {
        m_swappedObjectKey = key;
        m_swappedObject    = this->getInput< ::fwData::Object>(key);
    }

    //-------------------------------------------------------------------------
    virtual KeyConnectionsMap getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data1", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data2", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data3", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data4", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data5", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("dataGroup", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        return connections;
    }

    //-------------------------------------------------------------------------
    virtual void info(std::ostream& _sstream ) override
    {
        _sstream << "TestServiceImplementation";
    }

    //-------------------------------------------------------------------------

    const std::string& getSwappedObjectKey() const
    {
        return m_swappedObjectKey;
    }

    //-------------------------------------------------------------------------

    ::fwData::Object::csptr getSwappedObject() const
    {
        return m_swappedObject;
    }

    //-------------------------------------------------------------------------

private:
    std::string m_swappedObjectKey;
    ::fwData::Object::csptr m_swappedObject;
};

/**
 * @brief Service implementation for test
 */
class TestServiceImplementation2 : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro( (TestServiceImplementation2)(::fwServices::ut::TestService) );

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
    fwCoreServiceClassDefinitionsMacro( (TestSrvAutoconnect)(::fwServices::ut::TestService) );

    /// Keys to register Signal
    static const ::fwCom::Signals::SignalKeyType s_SIG_1;
    /// Keys to register Slot
    static const ::fwCom::Slots::SlotKeyType s_SLOT_1;

    /// Type os signal
    typedef ::fwCom::Signal< void (int)> MsgSentSignalType;

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

    virtual void updating() override
    {
        m_isUpdated = true;
    }
    //-------------------------------------------------------------------------

    virtual void info(std::ostream& _sstream ) override
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

    virtual IService::KeyConnectionsMap getAutoConnections() const override
    {
        KeyConnectionsMap connections;
        connections.push("data1", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("data2", ::fwData::Object::s_MODIFIED_SIG, s_SLOT_1);
        connections.push("data3", ::fwData::Object::s_MODIFIED_SIG, s_SLOT_1);
        connections.push("dataGroup0", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
        connections.push("dataGroup1", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

        return connections;
    }
    //-------------------------------------------------------------------------

private:

    int m_received;
};

/**
 * @brief Service with input and output
 */
class TestServiceWithData : public ::fwServices::IService
{
public:

    static const KeyType s_INPUT;
    static const KeyType s_OUTPUT;

    fwCoreServiceClassDefinitionsMacro( (TestServiceWithData)(::fwServices::IService) );
    TestServiceWithData() noexcept
    {
    }

    virtual ~TestServiceWithData() noexcept
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

    virtual void stopping() override;
    virtual void updating() override;
};

} //namespace ut
} //namespace fwServices
