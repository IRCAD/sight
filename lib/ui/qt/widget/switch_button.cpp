/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "switch_button.hpp"

#include <core/spy_log.hpp>

#include <QAbstractButton>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRect>
#include <QStyleOption>
namespace sight::ui::qt::widget
{

switch_button::switch_button(QWidget* _parent) :
    QAbstractButton(_parent),
    m_track_color_off(palette().color(QPalette::Button)),
    m_thumb_color(palette().color(QPalette::ButtonText)),
    m_current_track_color(isChecked() ? m_track_color_on : m_track_color_off)
{
    setCheckable(true);
    setObjectName("switch_button");
    setProperty("class", "switch_button");

    connect(
        this,
        &QAbstractButton::toggled,
        this,
        [this](bool _v)
        {
            Q_EMIT state_changed(_v ? Qt::Checked : Qt::Unchecked);
        });
}

//---------------------------------------------------------------------------
Qt::CheckState switch_button::check_state() const
{
    return isChecked() ? Qt::Checked : Qt::Unchecked;
}

//---------------------------------------------------------------------------

void switch_button::paintEvent(QPaintEvent* /*e*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QStyleOption opt;
    opt.initFrom(this);

    const int margin = 2;
    const int radius = 10;

    // Track design
    QPainterPath track_path;
    track_path.addRoundedRect(opt.rect.adjusted(margin, margin, -margin, -margin), radius, radius);
    painter.fillPath(track_path, m_current_track_color);

    const int top_rect    = height() - 35;
    const int width_rect  = 30;
    const int height_rect = 35;

    // Thumb design
    QRect thumb_rect(m_offset, top_rect / margin, width_rect, height_rect);
    QPainterPath thumb_path;
    thumb_path.addRoundedRect(thumb_rect, radius, radius);
    painter.fillPath(thumb_path, m_thumb_color);
}

//-------------------------------------------------------
QColor switch_button::current_track_color() const
{
    return m_current_track_color;
}

//-------------------------------------------------------
void switch_button::set_current_track_color(const QColor& _color)
{
    m_current_track_color = _color;
    update();
}

//-------------------------------------------------------

QColor switch_button::track_color_on() const
{
    return m_track_color_on;
}

//-------------------------------------------------------
void switch_button::set_track_color_on(const QColor& _color)
{
    m_track_color_on = _color;
    if(isChecked())
    {
        m_current_track_color = m_track_color_on;
    }

    update();
}

//--------------------------------------------------------
QColor switch_button::thumb_color() const
{
    return m_thumb_color;
}

//--------------------------------------------------------

void switch_button::set_check_state(const Qt::CheckState& _state)
{
    bool is_checked = (_state == Qt::Checked);
    if(is_checked == isChecked())
    {
        return;
    }

    setChecked(is_checked);
    m_offset              = m_end_offset[is_checked];
    m_current_track_color = is_checked ? m_track_color_on : m_track_color_off;
    update();
}

//--------------------------------------------------------
void switch_button::set_thumb_color(const QColor& _color)
{
    m_thumb_color = _color;
    update();
}

//---------------------------------------------------------
void switch_button::mousePressEvent(QMouseEvent* _event)
{
    if(_event->button() == Qt::LeftButton)
    {
        bool next_state = !isChecked();
        auto* group     = new QParallelAnimationGroup(this);

        auto* move_anim = new QPropertyAnimation(this, "offset");
        move_anim->setDuration(220);
        move_anim->setStartValue(m_offset);
        move_anim->setEndValue(m_end_offset[next_state]);

        auto* color_anim = new QPropertyAnimation(this, "current_track_color");
        color_anim->setStartValue(next_state ? m_track_color_off : m_track_color_on);
        color_anim->setEndValue(next_state ? m_track_color_on : m_track_color_off);
        color_anim->setDuration(220);

        group->addAnimation(move_anim);
        group->addAnimation(color_anim);

        connect(group, &QParallelAnimationGroup::finished, group, &QObject::deleteLater);
        group->start();
    }

    QAbstractButton::mousePressEvent(_event);
}

//-------------------------------------------------------
void switch_button::set_offset(int _offset)
{
    m_offset = _offset;
    update();
}

//------------------------------------------------------
void switch_button::resizeEvent(QResizeEvent* _event)
{
    QAbstractButton::resizeEvent(_event);

    constexpr int thumb_width = 30;
    constexpr int margin      = 2;

    m_end_offset[false] = margin;
    m_end_offset[true]  = width() - thumb_width - margin;

    m_offset = m_end_offset[isChecked()];
}

//--------------------------------------------------------
int switch_button::offset() const
{
    return m_offset;
}

//--------------------------------------------------------

} //namespace sight::ui::qt::widget
