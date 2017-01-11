/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>
#include <QHBoxLayout>
#include <QWidget>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include "uiIGTL/SConnexionStatusViewer.hpp"

fwServicesRegisterMacro (::gui::editor::IEditor, ::uiIGTL::SConnexionStatusViewer, ::fwData::Object);


namespace uiIGTL
{

const ::fwCom::Slots::SlotKeyType SConnexionStatusViewer::s_CONNECT_SLOT    = "connect";
const ::fwCom::Slots::SlotKeyType SConnexionStatusViewer::s_DISCONNECT_SLOT = "disconnect";

//-----------------------------------------------------------------------------

SConnexionStatusViewer::SConnexionStatusViewer()
{
    m_connectSlot = ::fwCom::newSlot (&SConnexionStatusViewer::connect, this);
    ::fwCom::HasSlots::m_slots (SConnexionStatusViewer::s_CONNECT_SLOT, m_connectSlot);

    m_disconnectSlot = ::fwCom::newSlot (&SConnexionStatusViewer::disconnect, this);
    ::fwCom::HasSlots::m_slots (SConnexionStatusViewer::s_DISCONNECT_SLOT, m_disconnectSlot);

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SConnexionStatusViewer::~SConnexionStatusViewer() throw()
{
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::configuring() throw (::fwTools::Failed)
{
    std::string valueStr;

    this->initialize();
    if (m_configuration->findConfigurationElement ("type"))
    {
        valueStr = m_configuration->findConfigurationElement ("type")->getValue();
        if (valueStr == "Server")
        {
            m_connexionType = SConnexionStatusViewer::SERVER;
        }
        else if (valueStr == "Client")
        {
            m_connexionType = SConnexionStatusViewer::CLIENT;
        }
        else
        {
            throw ::fwTools::Failed("Type supported is Client and Server");
        }
    }
    else
    {
        m_connexionType = SConnexionStatusViewer::CLIENT;
    }
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::starting() throw (::fwTools::Failed)
{
    QHBoxLayout* layout;

    this->create();
    m_status = new QLabel();
    layout   = new QHBoxLayout();
    layout->addWidget(m_status);
    if (m_connexionType == SConnexionStatusViewer::CLIENT)
    {
        m_status->setText("Disconnected");
    }
    else
    {
        m_status->setText("Stopped");
    }
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();

    container->setLayout(layout);
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::stopping() throw (::fwTools::Failed)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    SLM_ASSERT("container not instanced", qtContainer->getQtContainer());

    qtContainer->clean();
    this->destroy();
}

void SConnexionStatusViewer::updating() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::connect()
{
    if (m_connexionType == SConnexionStatusViewer::CLIENT)
    {
        m_status->setText("Connected");
    }
    else
    {
        m_status->setText("Started");
    }
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::disconnect()
{
    if (m_connexionType == SConnexionStatusViewer::CLIENT)
    {
        m_status->setText("Disconnected");
    }
    else
    {
        m_status->setText("Stopped");
    }
}

//-----------------------------------------------------------------------------

void SConnexionStatusViewer::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace uiIGTL


