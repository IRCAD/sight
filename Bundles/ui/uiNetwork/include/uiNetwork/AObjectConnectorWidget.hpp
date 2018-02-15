/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
