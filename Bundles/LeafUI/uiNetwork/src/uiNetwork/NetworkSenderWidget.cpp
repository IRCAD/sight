/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/NetworkSenderWidget.hpp"

#include <fwServices/op/Add.hpp>

#include <numeric>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

NetworkSenderWidget::NetworkSenderWidget (::fwData::Object::sptr obj,
                                          ConnectorModel const& connectorModel) :
    ANetworkServiceWidget(obj, connectorModel)
{
    m_portEditor = new QSpinBox();

    m_portEditor->setMinimum(0);
    m_portEditor->setMaximum(std::numeric_limits<std::uint16_t>::max());
    m_childLayout->addWidget(m_portEditor);

    for(std::string const& service :  connectorModel.getRegisteredSenders())
    {
        m_servicesBox->addItem(QString::fromStdString(service));
    }
    m_servicesBox->setCurrentIndex(0);
    m_currentServiceName = m_servicesBox->currentText().toStdString();
}

//-----------------------------------------------------------------------------

NetworkSenderWidget::~NetworkSenderWidget()
{
    this->onStop();
}

//-----------------------------------------------------------------------------

void NetworkSenderWidget::onStart()
{
    ConnectorModel::ServiceConfigurationContainer::const_iterator it;

    if (m_sender != NULL)
    {
        m_sender->start();
    }
    else
    {
        m_sender = ::fwServices::add< ::ioNetwork::INetworkSender >(m_obj, m_currentServiceName);
        if ((it = m_configs.find(m_currentServiceName)) != m_configs.end())
        {
            m_sender->setConfiguration(it->second);
            m_sender->configure();
        }
        SLM_ASSERT("Configuration must be modified to use the port set in the editor.", false);
        // The virtual function was removed, it is not necessary, the configuration can be modified
//        m_sender->setPort( m_portEditor->value());
        this->onStart();
    }
}

//-----------------------------------------------------------------------------

void NetworkSenderWidget::onStop()
{
    if (m_sender != NULL)
    {
        if (m_sender->isStarted())
        {
            m_sender->stop();
            m_sender = ::ioNetwork::INetworkSender::sptr();
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
