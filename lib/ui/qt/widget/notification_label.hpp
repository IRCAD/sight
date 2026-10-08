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

#pragma once

#include <sight/ui/qt/config.hpp>

#include <QIcon>
#include <QLabel>

namespace sight::ui::qt::widget
{

/**
 * @brief A label drawing its icon inside its own frame.
 *
 * This label paints its icon over its own frame, and reserves the room it takes so that the text, centered or not
 * and never runs under it.
 */
class SIGHT_UI_QT_CLASS_API_QT notification_label : public QLabel
{
Q_OBJECT

public:

    explicit notification_label(QWidget* _parent = nullptr, Qt::WindowFlags _f = Qt::WindowFlags()) :
        QLabel(_parent, _f)
    {
    }

    /// Draw this icon on the left of the text. A null icon removes it.
    void set_icon(const QIcon& _icon);

    /// Draw the icon at this side, in pixels. 0 follows the text height.
    void set_icon_size(int _size);

    /// The icon currently drawn, null when there is none.
    [[nodiscard]] const QIcon& icon() const
    {
        return m_icon;
    }

    /// The side, in pixels, the icon is actually drawn at.
    [[nodiscard]] int icon_side() const;

protected:

    /// Draw the icon over the frame the style just painted.
    void paintEvent(QPaintEvent* _event) override;

    /// Follow the text height when the style sheet changes the font.
    void changeEvent(QEvent* _event) override;

private:

    /// Reserve the room the icon takes on both sides, so that centered text stays centered.
    void update_margins();

    /// Distance between the frame and the icon, in pixels.
    static constexpr int GAP = 6;

    QIcon m_icon;
    int m_icon_size {0};
};

} // namespace sight::ui::qt::widget
