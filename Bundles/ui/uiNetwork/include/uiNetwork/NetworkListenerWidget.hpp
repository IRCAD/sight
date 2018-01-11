/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_NETWORKLISTENERWIDGET_HPP__
#define __UINETWORK_NETWORKLISTENERWIDGET_HPP__

#include <QLineEdit>
#include <QSpinBox>
#include "ioNetwork/INetworkListener.hpp"
#include "uiNetwork/ANetworkServiceWidget.hpp"
#include "uiNetwork/config.hpp"

namespace uiNetwork
{
/**
 *
 * @brief widget to handle a network listener
 */
class NetworkListenerWidget : public ANetworkServiceWidget
{
public:
    /**
     * @brief constructor
     */
    NetworkListenerWidget(::fwData::Object::sptr obj,
                          ConnectorModel const& connectorMode);

    /// Destructor
    ~NetworkListenerWidget();

private:
    /**
     * @brief add a listener service to the app and start it
     */
    void onStart();
    /**
     * @brief stop the listener service delete it to the app
     */
    void onStop();

private:
    /// listener service
    ::ioNetwork::INetworkListener::sptr m_listener;

    /// hostname editor widget
    QLineEdit* m_hostnameEditor;

    /// port editor widget
    QSpinBox* m_portEditor;

};
} //namespace uiNetwork

#endif // __UINETWORK_NETWORKLISTENERWIDGET_HPP__
