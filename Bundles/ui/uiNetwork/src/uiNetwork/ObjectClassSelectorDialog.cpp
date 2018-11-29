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
