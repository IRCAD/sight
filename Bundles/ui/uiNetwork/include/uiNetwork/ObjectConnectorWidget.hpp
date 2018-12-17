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

#ifndef __UINETWORK_OBJECTCONNECTORWIDGET_HPP__
#define __UINETWORK_OBJECTCONNECTORWIDGET_HPP__

#include "uiNetwork/AObjectConnectorWidget.hpp"
#include "uiNetwork/config.hpp"

#include <ioNetwork/INetworkSender.hpp>

#include <QWidget>

namespace uiNetwork
{
/**
 *
 * @brief class to add sender or listener to an object like connector
 */
class UINETWORK_CLASS_API ObjectConnectorWidget : public AObjectConnectorWidget
{
Q_OBJECT
public:

    /**
     * @brief constructor
     */
    UINETWORK_API ObjectConnectorWidget(QTreeWidget* root,
                                        ::fwData::Object::sptr obj,
                                        ConnectorModel const& connectorModel);

    /// Destructor
    UINETWORK_API virtual ~ObjectConnectorWidget();

    /**
     * @see AObjectConnectorWidget
     */
    UINETWORK_API bool match(::boost::regex const& regex);

    /**
     * @see AObjectConnectorWidget
     */
    UINETWORK_API void refresh(::boost::regex const& regex);

private Q_SLOTS:

    /**
     * @brief add NetworkSenderWidget to the tree item when user click on Add sender button
     */
    void onAddSender();

    /**
     * @brief add NetworkListenerWidget to the tree item when user click on Add listener button
     */
    void onAddListener();

    /**
     * @brief called when ObjectConnector receive a serviceRemoved from a child and remove the Widget
     */
    void onRemoveService(QTreeWidgetItem* item);

    /**
     * @brief add ioSelector service when user click on load data and when the user successfully loaded his data then the ioSelector is removed
     */
    void onLoadData();

};
} //namespace uiNetwork

#endif // __UINETWORK_OBJECTCONNECTORWIDGET_HPP__
