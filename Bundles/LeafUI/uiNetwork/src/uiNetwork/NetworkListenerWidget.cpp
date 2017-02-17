/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/NetworkListenerWidget.hpp"

#include <fwServices/op/Add.hpp>

#include <boost/integer_traits.hpp>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

NetworkListenerWidget::NetworkListenerWidget (::fwData::Object::sptr obj,
                                              ConnectorModel const& connectorModel) :
    ANetworkServiceWidget(obj, connectorModel)
{
    m_hostnameEditor = new QLineEdit();
    m_portEditor     = new QSpinBox();

    m_portEditor->setMinimum(0);
    m_portEditor->setMaximum(boost::integer_traits< ::boost::uint16_t>::const_max);
    m_childLayout->addWidget(m_hostnameEditor);
    m_childLayout->addWidget(m_portEditor);

    for(std::string const& service :  connectorModel.getRegisteredListeners())
    {
        m_servicesBox->addItem(QString::fromStdString(service));
    }
    m_servicesBox->setCurrentIndex(0);
    m_currentServiceName = m_servicesBox->currentText().toStdString();
}

//-----------------------------------------------------------------------------

NetworkListenerWidget::~NetworkListenerWidget()
{
    this->onStop();
}

//-----------------------------------------------------------------------------

void NetworkListenerWidget::onStart()
{
    ConnectorModel::ServiceConfigurationContainer::const_iterator it;

    if (m_listener != NULL)
    {
        m_listener->start();
    }
    else
    {
        m_listener = ::fwServices::add< ::ioNetwork::INetworkListener >(m_obj, m_currentServiceName);
        if ((it = m_configs.find(m_currentServiceName)) != m_configs.end())
        {
            m_listener->setConfiguration(it->second);
            m_listener->configure();
        }
        SLM_ASSERT("Configuration must be modified to use the hostname and port set in the editor.", false);
        // The virtual function was removed, it is not necessary, the configuration can be modified
//        m_listener->setHost(m_hostnameEditor->text().toStdString(), (::boost::uint16_t)m_portEditor->value());
        this->onStart();
    }
}

//-----------------------------------------------------------------------------

void NetworkListenerWidget::onStop()
{
    if (m_listener != NULL)
    {
        m_listener->stop();
        m_connections.disconnect();
        m_listener = ::ioNetwork::INetworkListener::sptr();
    }
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
