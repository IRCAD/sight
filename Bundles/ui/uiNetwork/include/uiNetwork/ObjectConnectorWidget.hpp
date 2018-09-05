/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
