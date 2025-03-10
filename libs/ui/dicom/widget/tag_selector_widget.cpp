/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/dicom/widget/tag_selector_widget.hpp"

#include <core/spy_log.hpp>

#include <dcmtk/dcmdata/dctag.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

namespace sight::ui::dicom::widget
{

//-----------------------------------------------------------------------------

tag_selector_widget::tag_selector_widget(QWidget* _parent) :
    QWidget(_parent),
    m_group_spin_box(new ui::dicom::widget::hex_spin_box()),
    m_element_spin_box(new ui::dicom::widget::hex_spin_box())
{
    // Create main layout
    auto* main_layout = new QVBoxLayout();
    this->setLayout(main_layout);
    main_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Add tag label
    m_tag_name_label = new QLabel("<b>Tag name :</b> ");
    main_layout->addWidget(m_tag_name_label);

    // Create bottom widget
    auto* bottom_widget = new QWidget();
    main_layout->addWidget(bottom_widget);
    auto* bottom_layout = new QHBoxLayout();
    bottom_widget->setLayout(bottom_layout);
    bottom_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Spin box
    bottom_layout->addWidget(new QLabel("<b>Group :</b>"));
    bottom_layout->addWidget(m_group_spin_box, 1);
    bottom_layout->addWidget(new QLabel("<b>Element :</b>"));
    bottom_layout->addWidget(m_element_spin_box, 1);

    // Update current name
    this->update_tag_name();

    // Connect signals/slots
    QObject::connect(m_group_spin_box, SIGNAL(valueChanged(int)), this, SLOT(update_tag_name(int)));
    QObject::connect(m_element_spin_box, SIGNAL(valueChanged(int)), this, SLOT(update_tag_name(int)));
}

//-----------------------------------------------------------------------------

tag_selector_widget::~tag_selector_widget()
{
    // Disconnect signales/slots
    QObject::disconnect(m_group_spin_box, SIGNAL(valueChanged(int)), this, SLOT(update_tag_name(int)));
    QObject::disconnect(m_element_spin_box, SIGNAL(valueChanged(int)), this, SLOT(update_tag_name(int)));
}

//-----------------------------------------------------------------------------

void tag_selector_widget::update_tag_name(int /*value*/)
{
    DcmTag tag(Uint16(m_group_spin_box->value()), Uint16(m_element_spin_box->value()));
    std::string text = "<b>Tag name :</b> " + std::string(tag.getTagName());
    m_tag_name_label->setText(text.c_str());
}

//-----------------------------------------------------------------------------

void tag_selector_widget::set_tag_value(const DcmTagKey& _tag)
{
    m_group_spin_box->setValue(_tag.getGroup());
    m_element_spin_box->setValue(_tag.getElement());
    this->update_tag_name();
}

//-----------------------------------------------------------------------------

DcmTagKey tag_selector_widget::get_tag()
{
    return {Uint16(m_group_spin_box->value()), Uint16(m_element_spin_box->value())};
}

} // namespace sight::ui::dicom::widget
