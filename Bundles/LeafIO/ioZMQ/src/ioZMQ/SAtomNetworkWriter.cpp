/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/SAtomNetworkWriter.hpp"

#include <fwData/Object.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::io::IWriter, ::ioZMQ::SAtomNetworkWriter, ::fwData::Object);

namespace ioZMQ
{

//-----------------------------------------------------------------------------

SAtomNetworkWriter::SAtomNetworkWriter()
{
}

//-----------------------------------------------------------------------------

SAtomNetworkWriter::~SAtomNetworkWriter() noexcept
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::configuring()
{
    ::io::IWriter::configuring();
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::starting()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::configureWithIHM()
{
    ::fwGui::dialog::InputDialog inputDialog;

    inputDialog.setTitle(m_windowTitle.empty() ? "Configure server" : m_windowTitle);
    m_host = inputDialog.getInput();
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::stopping()
{
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::updating()
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Object::sptr obj;
    ::fwData::Object::sptr response = ::fwData::String::New();

    try
    {
        obj      = this->getObject();
        m_socket = ::zmqNetwork::Socket::sptr(new ::zmqNetwork::Socket(::zmqNetwork::Socket::Server,
                                                                       ::zmqNetwork::Socket::Reply));
        m_socket->start(m_host);
        m_socket->receiveObject(response);
        m_socket->sendObject(obj);
        m_socket->receiveObject(response);
        m_socket->stop();

    }
    catch (std::exception& err)
    {
        m_socket->stop();
        msgDialog.showMessageDialog("Error", std::string(err.what()));
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SAtomNetworkWriter::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

void SAtomNetworkWriter::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ

