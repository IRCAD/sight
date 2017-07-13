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

SImageNetworkWriter::~SImageNetworkWriter() throw()
{
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::configuring() throw (::fwTools::Failed)
{
    ::io::IWriter::configuring();
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::starting() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::stopping() throw (::fwTools::Failed)
{
    m_socket->stop();
}

//-----------------------------------------------------------------------------

void SImageNetworkWriter::updating() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Image::sptr img;
    ::fwData::Object::sptr response = ::fwData::String::New();

    try
    {
        img      = this->getObject< ::fwData::Image >();
        m_socket = ::zmqNetwork::Socket::sptr(new ::zmqNetwork::Socket(::zmqNetwork::Socket::Server,
                                                                       ::zmqNetwork::Socket::Reply));
        m_socket->start(m_host);
        m_socket->receiveObject(response);
        m_socket->sendObject(img);
        m_socket->receiveObject(response);
    }
    catch (std::exception& err)
    {
        if (m_socket->isStarted())
        {
            m_socket->stop();
        }
        msgDialog.showMessageDialog("Error", std::string(err.what()));
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

void SImageNetworkWriter::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ

