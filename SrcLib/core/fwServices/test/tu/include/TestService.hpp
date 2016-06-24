/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_TESTSERVICE_HPP__
#define __FWSERVICES_UT_TESTSERVICE_HPP__

#include <fwServices/IService.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

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

    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) );
    TestService() throw() :
        m_isUpdated(false),
        m_isUpdatedMessage(false)
    {
    }

    virtual ~TestService() throw()
    {
    }

    virtual void configuring() throw( ::fwTools::Failed ) final
    {
    }
    virtual void starting() throw(::fwTools::Failed)override
    {
    }
    virtual void stopping() throw(::fwTools::Failed) final
    {
    }
    virtual void updating() throw(::fwTools::Failed)override
    {
    }
    virtual void info(std::ostream &_sstream )
    {
        _sstream << "TestService";
    }

    /// return true if the service is updated with update() method
    bool getIsUpdated() const
    {
        return m_isUpdated;
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

protected:
    bool m_isUpdated;
    bool m_isUpdatedMessage;
};

/**
 * @brief Service implementation for test
 */
class TestServiceImplementation : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementation)(::fwServices::ut::TestService) );

    /// Keys to register Signal
    static const ::fwCom::Signals::SignalKeyType s_MSG_SENT_SIG;
    /// Keys to register Slot
    static const ::fwCom::Slots::SlotKeyType s_RECEIVE_MSG_SLOT;

    /// Type os signal
    typedef ::fwCom::Signal< void (std::string)> MsgSentSignalType;

    //-------------------------------------------------------------------------
    TestServiceImplementation() throw()
    {
        newSignal<MsgSentSignalType>(s_MSG_SENT_SIG);
        newSlot(s_RECEIVE_MSG_SLOT, &TestServiceImplementation::receiveMsg, this);
    }
    //-------------------------------------------------------------------------
    virtual ~TestServiceImplementation() throw()
    {
    }

    //-------------------------------------------------------------------------
    virtual void starting() throw(::fwTools::Failed) final
    {
    }

    //-------------------------------------------------------------------------
    virtual void updating() throw(::fwTools::Failed)override
    {
        m_isUpdated = true;
    }

    //-------------------------------------------------------------------------
    virtual void swapping(const KeyType& key) throw(::fwTools::Failed)override
    {
        m_swappedObjectKey = key;
        m_swappedObject    = this->getInput< ::fwData::Object>(key);
    }

    //-------------------------------------------------------------------------
    void receiveMsg(std::string msg)
    {
        m_isUpdatedMessage = true;
    }

    //-------------------------------------------------------------------------
    virtual void info(std::ostream &_sstream ) override
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
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementation2)(::fwServices::ut::TestService) );

    //-------------------------------------------------------------------------
    TestServiceImplementation2() throw()
    {
    }
    //-------------------------------------------------------------------------
    virtual ~TestServiceImplementation2() throw()
    {
    }
};

/**
 * @brief Service implementation for autoconnect test
 */
class TestSrvAutoconnect : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestSrvAutoconnect)(::fwServices::ut::TestService) );

    /// Keys to register Signal
    static const ::fwCom::Signals::SignalKeyType s_SIG_1;
    /// Keys to register Slot
    static const ::fwCom::Slots::SlotKeyType s_SLOT_1;

    /// Type os signal
    typedef ::fwCom::Signal< void (int)> MsgSentSignalType;

    //-------------------------------------------------------------------------

    TestSrvAutoconnect() throw() :
        m_received(false)
    {
        newSignal<MsgSentSignalType>(s_SIG_1);
        newSlot(s_SLOT_1, &TestSrvAutoconnect::receiveSlot, this);
    }
    //-------------------------------------------------------------------------
    virtual ~TestSrvAutoconnect() throw()
    {
    }
    //-------------------------------------------------------------------------

    virtual void updating() throw(::fwTools::Failed)override
    {
        m_isUpdated = true;
    }
    //-------------------------------------------------------------------------

    virtual void info(std::ostream &_sstream ) override
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
        connections.push("dataGroup1", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

        return connections;
    }
    //-------------------------------------------------------------------------

private:

    int m_received;
};

} //namespace ut
} //namespace fwServices

#endif //__FWSERVICES_UT_TESTSERVICE_HPP__
