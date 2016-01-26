/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_TESTSERVICE_HPP__
#define __FWSERVICES_UT_TESTSERVICE_HPP__

#include <fwServices/IService.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <cppunit/extensions/HelperMacros.h>

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
    TestService() throw()
        :   m_isUpdated(false),
          m_isUpdatedMessage(false)
    {
    }

    virtual ~TestService() throw()
    {
    }

    virtual void configuring() throw( ::fwTools::Failed )
    {
    }
    virtual void starting() throw(::fwTools::Failed)
    {
    }
    virtual void stopping() throw(::fwTools::Failed)
    {
    }
    virtual void updating() throw(::fwTools::Failed)
    {
    }
    virtual void info(std::ostream &_sstream )
    {
        _sstream << "TestService";
    }

    /// return true if the service is updated with update() method
    bool getIsUpdated()
    {
        return m_isUpdated;
    }

    /// return true if the service is updated with update(msg) method
    bool getIsUpdatedMessage()
    {
        return m_isUpdatedMessage;
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

    TestServiceImplementation() throw()
    {
        newSignal<MsgSentSignalType>(s_MSG_SENT_SIG);
        newSlot(s_RECEIVE_MSG_SLOT, &TestServiceImplementation::receiveMsg, this);
    }
    virtual ~TestServiceImplementation() throw()
    {
    }

    virtual void configuring() throw( ::fwTools::Failed )
    {
    }
    virtual void starting() throw(::fwTools::Failed)
    {
    }
    virtual void stopping() throw(::fwTools::Failed)
    {
    }
    virtual void updating() throw(::fwTools::Failed)
    {
        m_isUpdated = true;
    }

    virtual void receiveMsg(std::string msg)
    {
        m_isUpdatedMessage = true;
    }

    virtual void info(std::ostream &_sstream )
    {
        _sstream << "TestServiceImplementation";
    }
};

} //namespace ut
} //namespace fwServices

#endif //__FWSERVICES_UT_TESTSERVICE_HPP__
