/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "ui/qt/widget/notification_label.hpp"

#include <QEvent>
#include <QFontMetrics>
#include <QPainter>

namespace sight::ui::qt::widget
{

//------------------------------------------------------------------------------

void notification_label::set_icon(const QIcon& _icon)
{
    m_icon = _icon;

    this->update_margins();
    this->update();
}

//------------------------------------------------------------------------------

void notification_label::set_icon_size(int _size)
{
    m_icon_size = _size;

    this->update_margins();
    this->update();
}

//------------------------------------------------------------------------------

int notification_label::icon_side() const
{
    // A text line high, so that the icon reads as part of the message and follows the style sheet font.
    return m_icon_size > 0 ? m_icon_size : QFontMetrics(this->font()).height();
}

//------------------------------------------------------------------------------

void notification_label::update_margins()
{
    // Margins rather than a style sheet: the popup reapplies the application one on this label, which would
    // wipe a padding set here. Reserved on both sides, so that centered text stays centered.
    const int reserved = m_icon.isNull() ? 0 : this->icon_side() + (2 * GAP);

    this->setContentsMargins(reserved, 0, reserved, 0);
}

//------------------------------------------------------------------------------

void notification_label::paintEvent(QPaintEvent* _event)
{
    QLabel::paintEvent(_event);

    if(m_icon.isNull())
    {
        return;
    }

    const int side  = this->icon_side();
    const auto area = this->rect();

    QPainter painter(this);
    painter.drawPixmap(
        QPoint(area.left() + GAP, area.center().y() - (side / 2)),
        m_icon.pixmap(side, side)
    );
}

//------------------------------------------------------------------------------

void notification_label::changeEvent(QEvent* _event)
{
    QLabel::changeEvent(_event);

    if(_event->type() == QEvent::FontChange || _event->type() == QEvent::StyleChange)
    {
        this->update_margins();
    }
}

} // namespace sight::ui::qt::widget
