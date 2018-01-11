/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_NETWORKSENDERWIDGET_HPP__
#define __UINETWORK_NETWORKSENDERWIDGET_HPP__

#include "uiNetwork/ANetworkServiceWidget.hpp"
#include "uiNetwork/config.hpp"

#include <ioNetwork/INetworkSender.hpp>

#include <QSpinBox>

namespace uiNetwork
{
/**
 *
 * @brief handle a network sender in runtime(add/remove network sender in runtime)
 */
class NetworkSenderWidget : public ANetworkServiceWidget
{
public:
    /**
     * @brief constructor
     */
    NetworkSenderWidget(::fwData::Object::sptr obj,
                        ConnectorModel const& connectorModel);

    /// Destructor
    ~NetworkSenderWidget();

private:
    /**
     * @brief create network sender, configure it, add to app and start it
     */
    void onStart();

    /**
     * @brief stop network sender and remove it to the app
     */
    void onStop();

private:
    /// listener service
    ::ioNetwork::INetworkSender::sptr m_sender;

    /// port editor widget
    QSpinBox* m_portEditor;

};
} //namespace uiNetwork

#endif // __UINETWORK_NETWORKSENDERWIDGET_HPP__
