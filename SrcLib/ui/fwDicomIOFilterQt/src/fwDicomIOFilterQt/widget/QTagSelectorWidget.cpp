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

#include "fwDicomIOFilterQt/widget/QTagSelectorWidget.hpp"

#include <fwCore/spyLog.hpp>
#include <fwDcmtkTools/Dictionary.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <dcmtk/dcmdata/dctag.h>

namespace fwDicomIOFilterQt
{
namespace widget
{


//-----------------------------------------------------------------------------

QTagSelectorWidget::QTagSelectorWidget(QWidget* parent) : QWidget(parent)
{
    // Load dictionary
    ::fwDcmtkTools::Dictionary::loadDictionary();

    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(QMargins(0,0,0,0));

    // Add tag label
    m_tagNameLabel = new QLabel("<b>Tag name :</b> ");
    mainLayout->addWidget(m_tagNameLabel);

    // Create bottom widget
    QWidget* bottomWidget = new QWidget();
    mainLayout->addWidget(bottomWidget);
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomWidget->setLayout(bottomLayout);
    bottomLayout->setContentsMargins(QMargins(0,0,0,0));

    // Spin box
    m_groupSpinBox   = new ::fwDicomIOFilterQt::widget::QHexSpinBox();
    m_elementSpinBox = new ::fwDicomIOFilterQt::widget::QHexSpinBox();

    bottomLayout->addWidget(new QLabel("<b>Group :</b>"));
    bottomLayout->addWidget(m_groupSpinBox, 1);
    bottomLayout->addWidget(new QLabel("<b>Element :</b>"));
    bottomLayout->addWidget(m_elementSpinBox, 1);

    // Update current name
    this->updateTagName();

    // Connect signals/slots
    QObject::connect(m_groupSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTagName(int)));
    QObject::connect(m_elementSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTagName(int)));
}

//-----------------------------------------------------------------------------

QTagSelectorWidget::~QTagSelectorWidget()
{
    // Disconnect signales/slots
    QObject::disconnect(m_groupSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTagName(int)));
    QObject::disconnect(m_elementSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTagName(int)));
}

//-----------------------------------------------------------------------------

void QTagSelectorWidget::updateTagName(int value)
{
    DcmTag tag(m_groupSpinBox->value(), m_elementSpinBox->value());
    std::string text = "<b>Tag name :</b> " + std::string(tag.getTagName());
    m_tagNameLabel->setText(text.c_str());
}

//-----------------------------------------------------------------------------

void QTagSelectorWidget::setTagValue(const DcmTagKey& tag)
{
    m_groupSpinBox->setValue(tag.getGroup());
    m_elementSpinBox->setValue(tag.getElement());
    this->updateTagName();
}

//-----------------------------------------------------------------------------

DcmTagKey QTagSelectorWidget::getTag()
{
    return DcmTagKey(m_groupSpinBox->value(), m_elementSpinBox->value());
}


} // namespace widget
} // namespace fwDicomIOFilterQt
