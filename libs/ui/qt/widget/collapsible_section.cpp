/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "collapsible_section.hpp"

#include "core/spy_log.hpp"

#include <QPropertyAnimation>

namespace sight::ui::qt::widget
{

collapsible_section::collapsible_section(const QString& _title, const int _animation_duration, QWidget* _parent) :
    QWidget(_parent),
    m_animation_duration(_animation_duration)
{
    m_toggle_button->setStyleSheet("QToolButton {border: none;}");
    m_toggle_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_toggle_button->setArrowType(Qt::ArrowType::RightArrow);
    m_toggle_button->setText(_title);
    m_toggle_button->setCheckable(true);
    m_toggle_button->setChecked(false);

    m_header_line->setFrameShape(QFrame::HLine);
    m_header_line->setFrameShadow(QFrame::Sunken);
    m_header_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_content_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    // Start out collapsed.
    m_content_area->setMaximumHeight(0);
    m_content_area->setMinimumHeight(0);

    // Let the entire widget grow and shrink with its content.
    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    m_toggle_animation->addAnimation(new QPropertyAnimation(m_content_area, "maximumHeight"));

    m_main_layout->setVerticalSpacing(0);
    m_main_layout->setContentsMargins(0, 0, 0, 0);

    int row = 0;
    m_main_layout->addWidget(m_toggle_button, row, 0, 1, 1, Qt::AlignLeft);
    m_main_layout->addWidget(m_header_line, row++, 2, 1, 1);
    m_main_layout->addWidget(m_content_area, row, 0, 1, 3);
    setLayout(m_main_layout);

    connect(m_toggle_button, &QToolButton::toggled, this, &collapsible_section::toggle);
}

//------------------------------------------------------------------------------

void collapsible_section::toggle(bool _expanded)
{
    m_toggle_button->setArrowType(_expanded ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    m_toggle_animation->setDirection(_expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    m_toggle_animation->start();

    this->m_is_expanded = _expanded;
}

//------------------------------------------------------------------------------

void collapsible_section::set_content_layout(QLayout* _content_layout)
{
    if(m_content_area->layout() != nullptr)
    {
        clean_layout_recursively(m_content_area->layout());
    }

    m_content_area->setLayout(_content_layout);
    m_collapsed_height = sizeHint().height() - m_content_area->maximumHeight();

    update_heights(false);
}

//------------------------------------------------------------------------------

void collapsible_section::set_title(QString _title)
{
    m_toggle_button->setText(_title);
}

//------------------------------------------------------------------------------

void collapsible_section::update_heights(bool _changing_content_while_unfolded)
{
    int content_height = m_content_area->layout()->sizeHint().height();

    for(int i = 0 ; i < m_toggle_animation->animationCount() - 1 ; ++i)
    {
        auto* section_animation = static_cast<QPropertyAnimation*>(m_toggle_animation->animationAt(i));
        section_animation->setDuration(m_animation_duration);
        section_animation->setStartValue(m_collapsed_height);
        section_animation->setEndValue(m_collapsed_height + content_height);
    }

    auto* content_animation =
        static_cast<QPropertyAnimation*>(m_toggle_animation->animationAt(m_toggle_animation->animationCount() - 1));
    content_animation->setDuration(m_animation_duration);
    content_animation->setStartValue(0);
    content_animation->setEndValue(content_height);

    m_toggle_animation->setDirection(m_is_expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);

    if(_changing_content_while_unfolded)
    {
        m_toggle_animation->start();
    }
}

//------------------------------------------------------------------------------

void collapsible_section::clean_layout_recursively(QLayout* _layout)
{
    while(QLayoutItem* const item = _layout->takeAt(0))
    {
        if(QWidget* const child_widget = item->widget(); child_widget != nullptr)
        {
            child_widget->deleteLater();
        }

        if(QLayout* const child_layout = item->layout(); child_layout != nullptr)
        {
            clean_layout_recursively(child_layout);
            child_layout->deleteLater();
        }

        delete item;
    }
}

} // namespace sight::ui::qt::widget
