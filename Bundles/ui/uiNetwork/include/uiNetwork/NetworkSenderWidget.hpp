/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
