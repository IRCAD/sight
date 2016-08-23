/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/ANetworkServiceWidget.hpp"

#include <Qt>
#include <QPushButton>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

ANetworkServiceWidget::ANetworkServiceWidget (::fwData::Object::sptr obj,
                                              ConnectorModel const& connectorModel) :
    QWidget(),
    m_obj (obj),
    m_configs (connectorModel.getServiceConfigurations())
{
    QPushButton* removeButton;
    QHBoxLayout* mainLayout;
    QCheckBox* startCheckBox;

    mainLayout    = new QHBoxLayout();
    m_childLayout = new QHBoxLayout();
    startCheckBox = new QCheckBox();
    m_servicesBox = new QComboBox();
    removeButton  = new QPushButton("Remove service");
    m_treeItem    = new QTreeWidgetItem();

    m_servicesBox->setCurrentIndex(0);

    mainLayout->addWidget(startCheckBox);
    mainLayout->addWidget(m_servicesBox);
    mainLayout->addLayout(m_childLayout);
    mainLayout->addWidget(removeButton);

    this->setLayout(mainLayout);

    QObject::connect(m_servicesBox, SIGNAL(currentTextChanged(QString const&)), this,
                     SLOT(onChangeService(QString const &)));
    QObject::connect(startCheckBox, SIGNAL(stateChanged(int)), this,
                     SLOT(onActivate(int)));
    QObject::connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveService()));

}

//-----------------------------------------------------------------------------

ANetworkServiceWidget::~ANetworkServiceWidget()
{
}

//-----------------------------------------------------------------------------

QTreeWidgetItem* ANetworkServiceWidget::getTreeItem()
{
    return m_treeItem;
}

//-----------------------------------------------------------------------------

void ANetworkServiceWidget::onActivate(int state)
{
    if (state == Qt::Checked)
    {
        this->onStart();
    }
    else
    {
        this->onStop();
    }
}

//-----------------------------------------------------------------------------

void ANetworkServiceWidget::onChangeService(QString const& service)
{
    m_currentServiceName = service.toStdString();
}

//-----------------------------------------------------------------------------

void ANetworkServiceWidget::onRemoveService()
{
    Q_EMIT serviceRemoved(m_treeItem);
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
