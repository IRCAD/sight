/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/SImageNetworkWriter.hpp"

#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwIO::IWriter, ::ioZMQ::SImageNetworkWriter, ::fwData::Image);

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
    ::fwIO::IWriter::configuring();
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::starting()
{
    FW_DEPRECATED_MSG("'ioZMQ' bundle and the associated services are no longer supported.", "19.0");
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
        std::string deviceName;
        ::fwData::Object::sptr response = m_socket->receiveObject(deviceName);
        m_socket->sendObject(img);
        response = m_socket->receiveObject(deviceName);
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

::fwIO::IOPathType SImageNetworkWriter::getIOPathType() const
{
    return ::fwIO::TYPE_NOT_DEFINED;
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
