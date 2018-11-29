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

#include "uiNetwork/ObjectConnectorWidget.hpp"

#include "uiNetwork/NetworkListenerWidget.hpp"
#include "uiNetwork/NetworkSenderWidget.hpp"

#include <fwGui/editor/IDialogEditor.hpp>

#include <fwServices/op/Add.hpp>

#include <uiIO/editor/SIOSelector.hpp>

#include <QPushButton>
#include <Qt>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

ObjectConnectorWidget::ObjectConnectorWidget(QTreeWidget* root,
                                             ::fwData::Object::sptr obj,
                                             ConnectorModel const& connectorModel) :
    AObjectConnectorWidget(root, obj, connectorModel)
{
    QHBoxLayout* layout;
    QLabel* idLabel;
    QLabel* typeLabel;
    QPushButton* addListenerButton;
    QPushButton* addSenderButton;
    QPushButton* loadDataButton;

    layout            = new QHBoxLayout();
    idLabel           = new QLabel(QString::fromStdString(m_obj->getLeafClassname()));
    typeLabel         = new QLabel(QString::fromStdString(m_obj->getID()));
    addListenerButton = new QPushButton("Add listener");
    addSenderButton   = new QPushButton("Add sender");
    loadDataButton    = new QPushButton("Load data");

    layout->addWidget(idLabel);
    layout->addWidget(typeLabel);
    layout->addWidget(addListenerButton);
    layout->addWidget(addSenderButton);
    layout->addWidget(loadDataButton);

    this->setLayout(layout);

    QObject::connect(addListenerButton, SIGNAL(clicked()), this, SLOT(onAddListener()));
    QObject::connect(addSenderButton, SIGNAL(clicked()), this, SLOT(onAddSender()));
    QObject::connect(loadDataButton, SIGNAL(clicked()), this, SLOT(onLoadData()));
}

//-----------------------------------------------------------------------------

void ObjectConnectorWidget::onLoadData()
{
    ::uiIO::editor::SIOSelector::sptr selectorService;

    selectorService =
        ::uiIO::editor::SIOSelector::dynamicCast(::fwServices::add< ::fwGui::editor::IDialogEditor >(m_obj,
                                                                                                     "::uiIO::editor::SIOSelector"));
    if (selectorService != NULL)
    {
        selectorService->setIOMode(::uiIO::editor::SIOSelector::READER_MODE);
        selectorService->configure();
        selectorService->start();
        selectorService->update();
        selectorService->stop();
    }
}

//-----------------------------------------------------------------------------

bool ObjectConnectorWidget::match(::boost::regex const& regex)
{
    ::boost::cmatch what;

    if (regex.empty() || regex.str().empty())
    {
        return true;
    }
    if (::boost::regex_match(m_obj->getLeafClassname().c_str(), what, regex)
        || ::boost::regex_match(m_obj->getID().c_str(), what, regex))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

void ObjectConnectorWidget::refresh(::boost::regex const& regex)
{

}

//-----------------------------------------------------------------------------

ObjectConnectorWidget::~ObjectConnectorWidget()
{
}

//-----------------------------------------------------------------------------

void ObjectConnectorWidget::onAddSender()
{
    NetworkSenderWidget* senderWidget;

    senderWidget = new NetworkSenderWidget(m_obj, m_connectorModel);
    m_treeItem->addChild(senderWidget->getTreeItem());
    m_root->setItemWidget(senderWidget->getTreeItem(), 0, senderWidget);

    QObject::connect(senderWidget, SIGNAL(serviceRemoved(QTreeWidgetItem*)), this,
                     SLOT(onRemoveService(QTreeWidgetItem*)));
}

//-----------------------------------------------------------------------------

void ObjectConnectorWidget::onRemoveService(QTreeWidgetItem* item)
{
    m_root->removeItemWidget(item, 0);
    m_treeItem->removeChild(item);
    delete item;
}

//-----------------------------------------------------------------------------

void ObjectConnectorWidget::onAddListener()
{
    NetworkListenerWidget* listenerWidget;

    listenerWidget = new NetworkListenerWidget(m_obj, m_connectorModel);
    m_treeItem->addChild(listenerWidget->getTreeItem());
    m_root->setItemWidget(listenerWidget->getTreeItem(), 0, listenerWidget);

    QObject::connect(listenerWidget, SIGNAL(serviceRemoved(QTreeWidgetItem*)), this,
                     SLOT(onRemoveService(QTreeWidgetItem*)));
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
