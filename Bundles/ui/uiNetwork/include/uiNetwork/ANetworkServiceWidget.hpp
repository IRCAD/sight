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

#ifndef __UINETWORK_ANETWORKSERVICEWIDGET_HPP__
#define __UINETWORK_ANETWORKSERVICEWIDGET_HPP__

#include "uiNetwork/ConnectorModel.hpp"
#include "uiNetwork/ObjectConnectorWidget.hpp"
#include "uiNetwork/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Object.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>

namespace uiNetwork
{
/**
 * @brief an abstract class for network service widgets
 */
class UINETWORK_CLASS_API ANetworkServiceWidget : public QWidget
{
Q_OBJECT
public:
    /**
     * @brief constructor
     */
    ANetworkServiceWidget(::fwData::Object::sptr obj,
                          ConnectorModel const& connectorModel);
    /// Destructor
    virtual ~ANetworkServiceWidget();

    /**
     * @brief get the tree item related to the widget
     *
     * @return tree item related to the widget
     */
    QTreeWidgetItem* getTreeItem();

Q_SIGNALS:
    /**
     * @brief signal sended when user click on remove service button
     *
     * @param[in] item irzm to remove in the tree
     */
    void serviceRemoved(QTreeWidgetItem* item);
private Q_SLOTS:

    /**
     * @brief called when user click on the checkbox
     *
     * @param[in] state change the state of the service
     */
    void onActivate(int state);

    /**
     * @brief called when user change the value on the combo box that contain services name
     *
     */
    void onChangeService(QString const& selectedService);

    /**
     * @brief called when user click on remove service button and emit serviceRemoved signal
     */
    void onRemoveService();

protected:
    /**
     * @brief called when the checkbox is checked must be implemented in child classes
     */
    virtual void onStart() = 0;

    /**
     * @brief called when the checkbox is unchecked must be implemented in child classes
     */
    virtual void onStop() = 0;

protected:

    /// child layout represent the part that will be changed
    QHBoxLayout* m_childLayout;

    /// services box
    QComboBox* m_servicesBox;
    /// current service name
    std::string m_currentServiceName;

    /// obj to connect
    ::fwData::Object::sptr m_obj;

    /// connectino between obj signal and service slots
    ::fwCom::helper::SigSlotConnection m_connections;

    /// Contains service configuration
    ConnectorModel::ServiceConfigurationContainer const& m_configs;

private:
    /// tree item
    QTreeWidgetItem* m_treeItem;

};
} //namespace uiNetwork

#endif // __UINETWORK_ANETWORKSERVICEWIDGET_HPP__
