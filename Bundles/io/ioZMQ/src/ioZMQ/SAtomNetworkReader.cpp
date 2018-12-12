/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ioZMQ/SAtomNetworkReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwIO::IReader, ::ioZMQ::SAtomNetworkReader, ::fwData::Object);

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
    ::fwIO::IReader::configuring();
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
    FW_DEPRECATED_MSG("'ioZMQ' bundle and the associated services are no longer supported.", "19.0");
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
        std::string deviceName;
        ::fwData::Object::sptr receiveObject = m_socket->receiveObject(deviceName);
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

::fwIO::IOPathType SAtomNetworkReader::getIOPathType() const
{
    return ::fwIO::TYPE_NOT_DEFINED;
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
