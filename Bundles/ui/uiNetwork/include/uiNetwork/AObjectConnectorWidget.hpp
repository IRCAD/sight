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

#ifndef __UINETWORK_AOBJECTCONNECTORWIDGET_HPP__
#define __UINETWORK_AOBJECTCONNECTORWIDGET_HPP__

#include "uiNetwork/config.hpp"
#include "uiNetwork/ConnectorModel.hpp"

#include <fwData/Object.hpp>

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <boost/regex.hpp>

namespace uiNetwork
{
/**
 *
 * @brief Abstract object connector widget
 */
class UINETWORK_CLASS_API AObjectConnectorWidget : public QWidget
{
public:
    /**
     * @brief constructor
     *
     * @param[in] root root tree
     * @param[in] obj obj to bind
     * @param[in] connectorModel contain all information needed to add/remove service to the object
     */
    UINETWORK_API AObjectConnectorWidget(QTreeWidget* root,
                                         ::fwData::Object::sptr obj,
                                         ConnectorModel const& connectorModel);

    /// Destructor
    UINETWORK_API virtual ~AObjectConnectorWidget();

    /**
     * @brief test if obj classname or id match with regex
     *
     * @return result of match
     */
    UINETWORK_API virtual bool match(::boost::regex const& regex) = 0;

    /**
     * @brief get qt tree item
     */
    UINETWORK_API QTreeWidgetItem* getTreeItem();

    /**
     * @brief refresh the widget connector
     */
    UINETWORK_API virtual void refresh(::boost::regex const& regex) = 0;

protected:

    /// Tree item
    QTreeWidgetItem* m_treeItem;

    /// Root tree
    QTreeWidget* m_root;

    /// obj to connect
    ::fwData::Object::sptr m_obj;

    /// ConnectorModel contain all information needed for ANetworkServiceWidget instances
    ConnectorModel const& m_connectorModel;
};
} //namespace uiNetwork

#endif // __UINETWORK_AOBJECTCONNECTORWIDGET_HPP__
