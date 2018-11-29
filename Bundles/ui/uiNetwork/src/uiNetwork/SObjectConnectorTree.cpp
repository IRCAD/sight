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

#include "uiNetwork/SObjectConnectorTree.hpp"

#include "uiNetwork/CompositeConnectorWidget.hpp"
#include "uiNetwork/ObjectConnectorWidget.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>
#include <fwData/Vector.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QLabel>
#include <QPushButton>
#include <QSize>
#include <QWidget>

fwServicesRegisterMacro(::fwGui::editor::IEditor, ::uiNetwork::SObjectConnectorTree, ::fwData::Object);

namespace uiNetwork
{

//-----------------------------------------------------------------------------

SObjectConnectorTree::SObjectConnectorTree() :
    QObject()
{
}

//-----------------------------------------------------------------------------

SObjectConnectorTree::~SObjectConnectorTree() noexcept
{
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::configuring()
{
    this->initialize();
    m_model.initialize(m_configuration, this->getObject());
    if (m_configuration->findConfigurationElement("filter"))
    {
        m_filter = ::boost::regex(m_configuration->findConfigurationElement("filter")->getValue());
    }
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::starting()
{
    FW_DEPRECATED_MSG("'uiNetwork' bundle and the associated services are no longer supported.", "19.0");
    QHBoxLayout* searchLayout;
    QPushButton* refreshButton;
    QLabel* searchLabel;
    QWidget* container;

    this->create();
    searchLabel    = new QLabel("Filter : ");
    m_objTree      = new QTreeWidget();
    searchLayout   = new QHBoxLayout();
    refreshButton  = new QPushButton("Refresh");
    m_filterEditor = new QLineEdit();
    m_layout       = new QVBoxLayout();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_filterEditor);
    searchLayout->addWidget(refreshButton);
    m_layout->addLayout(searchLayout);
    m_layout->addWidget(m_objTree);

    qtContainer->setLayout(m_layout);

    m_filterEditor->setText(QString(m_filter.expression()));
    m_objTree->setHeaderLabel("Data Objects");
    this->initRootConnector();
    this->refreshTree();

    QObject::connect(m_filterEditor, SIGNAL(textChanged(QString const&)), this, SLOT(onFilterChange(QString const&)));
    QObject::connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshTree()));
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::refreshTree()
{
    m_root->getTreeItem()->setHidden(!m_root->match(m_filter));
    m_root->refresh(m_filter);
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::initRootConnector()
{
    ::fwData::Object::sptr obj = this->getObject();

    if (obj->getClassname() == ::fwData::Composite::classname()
        || obj->getClassname() == ::fwData::Vector::classname())
    {
        m_root = new CompositeConnectorWidget(m_objTree, obj, m_model);
    }
    else
    {
        m_root = new ObjectConnectorWidget(m_objTree, obj, m_model);
    }
    m_objTree->addTopLevelItem(m_root->getTreeItem());
    m_objTree->setItemWidget(m_root->getTreeItem(), 0, m_root);
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::onFilterChange(QString const& str)
{
    try
    {
        m_filter.set_expression(str.toStdString());
        this->refreshTree();
    }
    catch (::boost::regex_error const&)
    {

    }
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::updating()
{
}

//-----------------------------------------------------------------------------

void SObjectConnectorTree::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

}  //namespace uiNetwork
