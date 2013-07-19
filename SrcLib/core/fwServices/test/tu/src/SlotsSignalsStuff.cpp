/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/registry/macros.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwCom/Connection.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>


#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include "SlotsSignalsStuff.hpp"

namespace fwServices
{
namespace ut
{

fwDataRegisterMacro(Buffer);

fwServicesRegisterMacro( ::fwServices::ut::IBasicTest , ::fwServices::ut::SBasicTest , ::fwServices::ut::Buffer ) ;
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest , ::fwServices::ut::SReaderTest , ::fwServices::ut::Buffer ) ;
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest , ::fwServices::ut::SShowTest , ::fwServices::ut::Buffer ) ;
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest , ::fwServices::ut::SReader2Test , ::fwServices::ut::Buffer ) ;
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest , ::fwServices::ut::SShow2Test , ::fwServices::ut::Buffer ) ;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SBasicTest::SBasicTest() : m_updateFinished(false), m_swapFinished(false)
{}

//------------------------------------------------------------------------------

void SBasicTest::starting() throw ( ::fwTools::Failed )
{
    ::boost::this_thread::sleep_for(m_startRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::stopping() throw ( ::fwTools::Failed )
{
    ::boost::this_thread::sleep_for(m_stopRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::swapping() throw ( ::fwTools::Failed )
{
    ::boost::this_thread::sleep_for(m_swapRetarder);
    m_swapFinished = true;
}

//------------------------------------------------------------------------------

void SBasicTest::updating() throw ( ::fwTools::Failed )
{
    ::boost::this_thread::sleep_for(m_updateRetarder);
    m_updateFinished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SReaderTest::updating() throw ( ::fwTools::Failed )
{
    Buffer::sptr buff = this->getObject< Buffer >();

    // Emit object Modified
    ObjectMsg::sptr msg = ObjectMsg::New();
    msg->addEvent(ObjectMsg::NEW_OBJECT);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = buff->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    fwServicesBlockAndNotifyMsgMacro(this->getLightID(), sig, msg, m_slotReceive)
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SShowTest::s_CHANGE_SLOT = "change";

//------------------------------------------------------------------------------

SShowTest::SShowTest() : m_receiveCount(0), m_changeCount(0)
{
    m_slotChange = ::fwCom::newSlot( &SShowTest::change, this ) ;
    ::fwCom::HasSlots::m_slots( s_CHANGE_SLOT, m_slotChange );
#ifdef COM_LOG
    m_slotChange->setID( s_CHANGE_SLOT );
#endif
}

//------------------------------------------------------------------------------

void SShowTest::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    Buffer::sptr buffer = this->getObject<Buffer>();
    ::boost::this_thread::sleep_for(m_receiveRetarder);
    ::fwData::mt::ObjectWriteLock lock(buffer);
    ++m_receiveCount;
}

//------------------------------------------------------------------------------

void SShowTest::change()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    ++m_changeCount;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SReader2Test::s_CHANGED_SIG = "changed";

//------------------------------------------------------------------------------

SReader2Test::SReader2Test()
{
    m_sigChanged = ChangedSignalType::New();
#ifdef COM_LOG
    m_sigChanged->setID( s_CHANGED_SIG );
#endif
    // Register
    ::fwCom::HasSignals::m_signals( s_CHANGED_SIG,  m_sigChanged);
}

//------------------------------------------------------------------------------

void SReader2Test::updating() throw ( ::fwTools::Failed )
{
    // Emit object Modified
    SReader2Test::ChangedSignalType::sptr sig;
    sig = this->signal< SReader2Test::ChangedSignalType >( SReader2Test::s_CHANGED_SIG );
    fwServicesNotifyMacro(this->getLightID(), sig, ());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SShow2Test::SShow2Test() : m_receiveCount(0)
{}

//------------------------------------------------------------------------------

void SShow2Test::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    Buffer::sptr buffer = this->getObject<Buffer>();
    ::boost::this_thread::sleep_for(m_receiveRetarder);
    ::fwData::mt::ObjectWriteLock lock(buffer);
    ++m_receiveCount;

    this->updating();
}

//------------------------------------------------------------------------------

void SShow2Test::updating() throw ( ::fwTools::Failed )
{
    Buffer::sptr buff = this->getObject< Buffer >();

    // Emit object Modified
    ObjectMsg::sptr msg = ObjectMsg::New();
    msg->addEvent(ObjectMsg::NEW_OBJECT);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = buff->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );
    fwServicesBlockAndNotifyMsgMacro(this->getLightID(), sig, msg, m_slotReceive);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
