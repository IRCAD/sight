/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/dicom/widget/QTagSelectorWidget.hpp"

#include <core/spy_log.hpp>

#include <dcmtk/dcmdata/dctag.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace sight::ui::dicom::widget
{

//-----------------------------------------------------------------------------

QTagSelectorWidget::QTagSelectorWidget(QWidget* _parent) :
    QWidget(_parent),
    m_groupSpinBox(new ui::dicom::widget::QHexSpinBox()),
    m_elementSpinBox(new ui::dicom::widget::QHexSpinBox())
{
    // Create main layout
    auto* main_layout = new QVBoxLayout();
    this->setLayout(main_layout);
    main_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Add tag label
    m_tagNameLabel = new QLabel("<b>Tag name :</b> ");
    main_layout->addWidget(m_tagNameLabel);

    // Create bottom widget
    auto* bottom_widget = new QWidget();
    main_layout->addWidget(bottom_widget);
    auto* bottom_layout = new QHBoxLayout();
    bottom_widget->setLayout(bottom_layout);
    bottom_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Spin box
    bottom_layout->addWidget(new QLabel("<b>Group :</b>"));
    bottom_layout->addWidget(m_groupSpinBox, 1);
    bottom_layout->addWidget(new QLabel("<b>Element :</b>"));
    bottom_layout->addWidget(m_elementSpinBox, 1);

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

void QTagSelectorWidget::updateTagName(int /*value*/)
{
    DcmTag tag(Uint16(m_groupSpinBox->value()), Uint16(m_elementSpinBox->value()));
    std::string text = "<b>Tag name :</b> " + std::string(tag.getTagName());
    m_tagNameLabel->setText(text.c_str());
}

//-----------------------------------------------------------------------------

void QTagSelectorWidget::setTagValue(const DcmTagKey& _tag)
{
    m_groupSpinBox->setValue(_tag.getGroup());
    m_elementSpinBox->setValue(_tag.getElement());
    this->updateTagName();
}

//-----------------------------------------------------------------------------

DcmTagKey QTagSelectorWidget::getTag()
{
    return {Uint16(m_groupSpinBox->value()), Uint16(m_elementSpinBox->value())};
}

} // namespace sight::ui::dicom::widget
