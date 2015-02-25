/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SImageNetworkReader.hpp"

#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>

#include <fwCore/HiResTimer.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/InputDialog.hpp>

#include <boost/type.hpp>

fwServicesRegisterMacro (::io::IReader, ::ioIGTL::SImageNetworkReader, ::fwData::Image);


namespace ioIGTL
{

//-----------------------------------------------------------------------------

SImageNetworkReader::SImageNetworkReader()
{
}

//-----------------------------------------------------------------------------

SImageNetworkReader::~SImageNetworkReader() throw()
{
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::configuring() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::configureWithIHM() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;
    std::string portStr;
    std::string addr;
    std::string::size_type splitPosition;
    ::fwGui::dialog::InputDialog inputDialog;

    inputDialog.setTitle ("Enter server address");
    addr          = inputDialog.getInput();
    splitPosition = addr.find (':');
    if (splitPosition == std::string::npos)
    {
        msgDialog.showMessageDialog ("Error", "Server address is not formatted correctly hostname:port");
    }
    else
    {
        m_hostname = addr.substr (0, splitPosition);
        portStr    = addr.substr (splitPosition + 1, addr.size());
        m_port     = ::boost::lexical_cast<boost::uint16_t> (portStr);
    }
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::starting() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::stopping() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::updating() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    ::fwComEd::ImageMsg::sptr msg;
    ::fwData::Object::sptr obj;

    try
    {
        m_client.connect (m_hostname, m_port);
        msg = ::fwComEd::ImageMsg::New();
        obj = this->getObject();
        m_client.receiveObject(obj);
        m_client.disconnect();
        ::ioNetwork::ObjectUpdateNotifier::notifyUpdate(this->getSptr());
    }
    catch (std::exception &err)
    {
        msgDialog.showMessageDialog ("Error", err.what());
    }

}

//-----------------------------------------------------------------------------

void SImageNetworkReader::receiving (::fwServices::ObjectMsg::csptr _msg) throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

::io::IOPathType SImageNetworkReader::getIOPathType() const
{
    return ::io::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

void SImageNetworkReader::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL


