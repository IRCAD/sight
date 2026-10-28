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
 * @brief A label drawing an icon right before its text, inside its own frame.
 *
 * The icon and the text are aligned as one group, following the label alignment. QLabel still paints the text:
 * this label only reserves the icon room on the left of its contents, measures the text and paints the icon
 * just before it.
 */
class SIGHT_UI_QT_CLASS_API_QT notification_label : public QLabel
{
Q_OBJECT

public:

    /// Space between the icon and the text, in pixels.
    static constexpr int GAP = 10;

    /// Share of the available height an automatic icon takes, leaving some room above and below.
    static constexpr int AUTO_ICON_PERCENT = 80;

    explicit notification_label(QWidget* _parent = nullptr, Qt::WindowFlags _f = Qt::WindowFlags()) :
        QLabel(_parent, _f)
    {
    }

    /// Draw this icon before the text. A null icon removes it.
    void set_icon(const QIcon& _icon);

    /// Draw the icon at this side, in pixels. 0 takes the available height, at least the text height.
    void set_icon_size(int _size);

    /// The icon currently drawn, null when there is none.
    [[nodiscard]] const QIcon& icon() const
    {
        return m_icon;
    }

    /// The side, in pixels, the icon is actually drawn at, 0 without icon.
    [[nodiscard]] int icon_side() const;

    /// Where the icon is drawn, empty without icon.
    [[nodiscard]] QRect icon_rect() const;

    /// Where the text is drawn: its horizontal extent, over the whole contents height.
    [[nodiscard]] QRect text_rect() const;

    /// Room the icon takes on the left of this label at the given height, gap included. 0 without icon.
    [[nodiscard]] int icon_room(int _height) const;

    [[nodiscard]] int heightForWidth(int _width) const override;
    [[nodiscard]] QSize sizeHint() const override;
    [[nodiscard]] QSize minimumSizeHint() const override;

protected:

    void paintEvent(QPaintEvent* _event) override;
    void resizeEvent(QResizeEvent* _event) override;
    void changeEvent(QEvent* _event) override;

private:

    struct placement
    {
        int side {0};
        QMargins margins {};
        QRect icon {};
        QRect text {};
    };

    [[nodiscard]] placement compute_placement() const;

    /// Apply the contents margins of the current placement.
    void update_margins();

    /// Margins the style takes for its border and padding, which QFrame keeps in the contents margins.
    [[nodiscard]] QMargins frame_margins(const QRect& _rect) const;

    /// Size of the text laid out at this width, as QLabel lays it out.
    [[nodiscard]] QSizeF measure_text(int _width) const;

    /// Icon side wanted for this contents height, before checking that the text still fits.
    [[nodiscard]] int wanted_side(int _height) const;

    /// Width QLabel's hints must leave out: they are computed as if the icon had its smallest size.
    [[nodiscard]] int hint_extra_width() const;

    /// Height an explicit icon needs, 0 otherwise.
    [[nodiscard]] int icon_min_height() const;

    QIcon m_icon;
    int m_icon_size {0};
};

} // namespace sight::ui::qt::widget
