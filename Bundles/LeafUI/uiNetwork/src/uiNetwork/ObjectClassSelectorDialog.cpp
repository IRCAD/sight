/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/ObjectClassSelectorDialog.hpp"

#include <fwData/registry/detail.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QObject>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

ObjectClassSelectorDialog::ObjectClassSelectorDialog() :
    QDialog()
{
    std::vector<std::string> classnames;
    QHBoxLayout* buttonLayout;
    QVBoxLayout* mainLayout;
    QPushButton* validateButton;
    QPushButton* cancelButton;

    buttonLayout   = new QHBoxLayout();
    mainLayout     = new QVBoxLayout();
    validateButton = new QPushButton("Validate");
    cancelButton   = new QPushButton("Cancel");
    m_classnameBox = new QComboBox();

    classnames = ::fwData::registry::get()->getFactoryKeys();
    for(std::string const& classname :  classnames)
    {
        m_classnameBox->addItem(QString::fromStdString(classname));
    }

    buttonLayout->addWidget(validateButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addWidget(m_classnameBox);
    mainLayout->addLayout(buttonLayout);

    this->setModal(true);
    this->setLayout(mainLayout);

    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidateSelection()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

//-----------------------------------------------------------------------------

ObjectClassSelectorDialog::~ObjectClassSelectorDialog()
{
}

//-----------------------------------------------------------------------------

void ObjectClassSelectorDialog::onValidateSelection()
{
    m_classnameSelected = m_classnameBox->currentText().toStdString();
    if (!m_classnameSelected.empty())
    {
        Q_EMIT classnameSelected(m_classnameSelected);
    }
    this->close();
}

//-----------------------------------------------------------------------------

std::string const& ObjectClassSelectorDialog::getClassnameSelected() const
{
    return m_classnameSelected;
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
