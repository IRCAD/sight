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
