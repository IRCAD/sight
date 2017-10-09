/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/SAtomNetworkReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::io::IReader, ::ioZMQ::SAtomNetworkReader, ::fwData::Object);

namespace ioZMQ
{

//-----------------------------------------------------------------------------

SAtomNetworkReader::SAtomNetworkReader()
{
}

//-----------------------------------------------------------------------------

SAtomNetworkReader::~SAtomNetworkReader() noexcept
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::configuring()
{
    ::io::IReader::configuring();
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::configureWithIHM()
{
    ::fwGui::dialog::InputDialog inputDialog;

    inputDialog.setTitle(m_windowTitle.empty() ? "Enter hostname" : m_windowTitle);
    m_host = inputDialog.getInput();
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::starting()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::stopping()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::updating()
{
    ::fwData::String::sptr request;

    try
    {
        ::fwData::Object::sptr obj = this->getObject();
        request                    = ::fwData::String::New();
        request->setValue("GET ATOM");
        m_socket = ::zmqNetwork::Socket::sptr(new ::zmqNetwork::Socket(::zmqNetwork::Socket::Client,
                                                                       ::zmqNetwork::Socket::Request));
        m_socket->start(m_host);
        m_socket->sendObject(request);
        ::fwData::Object::sptr receiveObject = m_socket->receiveObject();
        m_socket->sendObject(request);
        m_socket->stop();

        if(receiveObject)
        {
            obj->shallowCopy(receiveObject);
            ::fwData::Object::ModifiedSignalType::sptr sig;
            sig = obj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    catch (std::exception& err)
    {
        if (m_socket->isStarted())
        {
            m_socket->sendObject(request);
        }
        m_socket->stop();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", err.what());
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SAtomNetworkReader::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

void SAtomNetworkReader::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ

