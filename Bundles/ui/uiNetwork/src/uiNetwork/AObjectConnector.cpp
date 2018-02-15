/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/AObjectConnectorWidget.hpp"

namespace uiNetwork
{
AObjectConnectorWidget::AObjectConnectorWidget(QTreeWidget* root,
                                               ::fwData::Object::sptr obj,
                                               ConnectorModel const& connectorModel) :
    QWidget(),
    m_root(root),
    m_obj(obj),
    m_connectorModel(connectorModel)
{
    m_treeItem = new QTreeWidgetItem();
}

AObjectConnectorWidget::~AObjectConnectorWidget()
{

}

QTreeWidgetItem* AObjectConnectorWidget::getTreeItem()
{
    return m_treeItem;
}
} //namespace uiNetwork
