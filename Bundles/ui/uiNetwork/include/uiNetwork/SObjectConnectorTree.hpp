/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "uiNetwork/AObjectConnectorWidget.hpp"
#include "uiNetwork/config.hpp"
#include "uiNetwork/ConnectorModel.hpp"
#include "uiNetwork/ObjectConnectorWidget.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <boost/regex.hpp>

#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace uiNetwork
{

/**
 * @brief This editors allows to create listener/sender services on the object.
 *
 * If the current object is a Composite or a Vector, this editors shows the lists of its sub-objects and allows to add
 * sub-objects. On each sub-objects, we could create listener/sender services.
 */
class UINETWORK_CLASS_API SObjectConnectorTree :
    public QObject,
    public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SObjectConnectorTree) (::fwGui::editor::IEditor));

    /**
     * @brief configure the service and initialize the ConnectorModel
     *
     * @code{.xml}
        <service uid="${GENERIC_UID}_object_connector_view" type="::fwGui::editor::IEditor"
     * impl="::uiNetwork::SObjectConnectorTree" autoConnect="yes">
            <filter>T.*</filter>
            <config impl="::ioZMQ::SZeroMQSender">
                <!-- service configuration -->
            </config>
            <config impl="::ioZMQ::SZeroMQListener">
               <!-- service configuration -->
            </config>
        </service>
       @endcode
     *
     * - \b filter (optional): regex to filter objects
     * - \b config (optional): specific configuration of the listener/sender service to create
     *   - \b impl: implementation of service to configure
     */
    UINETWORK_API void configuring() override;

    /**
     * @brief construct the IHM and the Data object tree
     */
    UINETWORK_API void starting() override;

    /**
     * @brief destruct the IHM
     */
    UINETWORK_API void stopping() override;

    /// Overrides
    UINETWORK_API void updating() override;

    /// Overrides
    UINETWORK_API void swapping() override;

    /// Constructor
    UINETWORK_API SObjectConnectorTree();

    /// Destructor
    UINETWORK_API ~SObjectConnectorTree() noexcept;

private:

    void initRootConnector();

private Q_SLOTS:

    /**
     * @brief slot called when user change the filter in the m_filterEditor
     */
    void onFilterChange(QString const& str);

    /**
     * @brief refresh the tree widgets
     */
    void refreshTree();

private:
    /// main layout
    QVBoxLayout* m_layout;

    /// filter editor contain
    QLineEdit* m_filterEditor;

    /// Tree widget contain all object connector widgets
    QTreeWidget* m_objTree;

    /// root object connector
    AObjectConnectorWidget* m_root;

    /// regex to filter object can be connected
    ::boost::regex m_filter;

    /// Model contain all information needed by ANetworkServiceWidget instances
    ConnectorModel m_model;

};

} // namespace uiNetwork
