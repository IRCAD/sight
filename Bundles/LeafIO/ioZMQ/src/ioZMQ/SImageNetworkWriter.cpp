/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/SImageNetworkWriter.hpp"

#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::io::IWriter, ::ioZMQ::SImageNetworkWriter, ::fwData::Image);

namespace ioZMQ
{

SImageNetworkWriter::SImageNetworkWriter()
{
}

//-----------------------------------------------------------------------------

SImageNetworkWriter::~SImageNetworkWriter() noexcept
{
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::configuring()
{
    ::io::IWriter::configuring();
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::starting()
{
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::stopping()
{
    m_socket->stop();
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::updating()
{
    try
    {
        ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();

        m_socket = std::make_shared< ::zmqNetwork::Socket >(::zmqNetwork::Socket::Server, ::zmqNetwork::Socket::Reply);
        m_socket->start(m_host);
        ::fwData::Object::sptr response = m_socket->receiveObject();
        m_socket->sendObject(img);
        response = m_socket->receiveObject();
    }
    catch (std::exception& err)
    {
        if (m_socket->isStarted())
        {
            m_socket->stop();
        }
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", std::string(err.what()));
    }
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::configureWithIHM()
{
    ::fwGui::dialog::InputDialog inputDialog;

    inputDialog.setTitle(m_windowTitle.empty() ? "Configure server" : m_windowTitle);
    m_host = inputDialog.getInput();
}

//-----------------------------------------------------------------------------

::io::IOPathType SImageNetworkWriter::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ

