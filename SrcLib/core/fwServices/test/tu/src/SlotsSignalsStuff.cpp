/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SlotsSignalsStuff.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwCom/Connection.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/registry/macros.hpp>

namespace fwServices
{
namespace ut
{

fwDataRegisterMacro(Buffer);

fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SBasicTest );
fwServicesRegisterObjectMacro( ::fwServices::ut::SBasicTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SReaderTest );
fwServicesRegisterObjectMacro( ::fwServices::ut::SReaderTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SShowTest );
fwServicesRegisterObjectMacro( ::fwServices::ut::SShowTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SReader2Test );
fwServicesRegisterObjectMacro( ::fwServices::ut::SReader2Test, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SShow2Test );
fwServicesRegisterObjectMacro( ::fwServices::ut::SShow2Test, ::fwServices::ut::Buffer );

const ::fwServices::IService::KeyType IBasicTest::s_BUFFER_INOUT = "buffer";

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SBasicTest::SBasicTest() :
    m_updateFinished(false),
    m_swapFinished(false)
{
}

//------------------------------------------------------------------------------

void SBasicTest::starting()
{
    std::this_thread::sleep_for(m_startRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::stopping()
{
    std::this_thread::sleep_for(m_stopRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::swapping(const KeyType& )
{
    std::this_thread::sleep_for(m_swapRetarder);
    m_swapFinished = true;
}

//------------------------------------------------------------------------------

void SBasicTest::updating()
{
    std::this_thread::sleep_for(m_updateRetarder);
    m_updateFinished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SReaderTest::updating()
{
    Buffer::sptr buff = this->getInOut< Buffer >(s_BUFFER_INOUT);

    // Emit object Modified
    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = buff->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );

    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SShowTest::s_CHANGE_SLOT = "change";

//------------------------------------------------------------------------------

SShowTest::SShowTest() :
    m_receiveCount(0),
    m_changeCount(0)
{
    m_slotChange = ::fwCom::newSlot( &SShowTest::change, this );
    ::fwCom::HasSlots::m_slots( s_CHANGE_SLOT, m_slotChange );
}

//------------------------------------------------------------------------------

void SShowTest::updating()
{
    Buffer::sptr buffer = this->getInOut<Buffer>(s_BUFFER_INOUT);
    std::this_thread::sleep_for(m_receiveRetarder);
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

::fwServices::IService::KeyConnectionsMap SShowTest::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_BUFFER_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SReader2Test::s_CHANGED_SIG = "changed";

//------------------------------------------------------------------------------

SReader2Test::SReader2Test()
{
    m_sigChanged = ChangedSignalType::New();
    // Register
    ::fwCom::HasSignals::m_signals( s_CHANGED_SIG,  m_sigChanged);
}

//------------------------------------------------------------------------------

void SReader2Test::updating()
{
    // Emit object Modified
    SReader2Test::ChangedSignalType::sptr sig;
    sig = this->signal< SReader2Test::ChangedSignalType >( SReader2Test::s_CHANGED_SIG );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SShow2Test::s_UPDATE_BUFFER_SLOT = "updateBuffer";

//------------------------------------------------------------------------------

SShow2Test::SShow2Test() :
    m_receiveCount(0)
{
    newSlot(s_UPDATE_BUFFER_SLOT, &SShow2Test::updateBuffer, this);
}

//------------------------------------------------------------------------------

void SShow2Test::updating()
{
    Buffer::sptr buff = this->getInOut< Buffer >(s_BUFFER_INOUT);

    // Emit object Modified
    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = buff->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_BUFFER_SLOT)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SShow2Test::updateBuffer()
{
    Buffer::sptr buffer = this->getInOut<Buffer>(s_BUFFER_INOUT);
    std::this_thread::sleep_for(m_receiveRetarder);
    ::fwData::mt::ObjectWriteLock lock(buffer);
    ++m_receiveCount;

    this->updating();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
