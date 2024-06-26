/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <QGraphicsView>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include <QToolBar>

namespace sight::ui::qt::widget
{

class SIGHT_UI_QT_CLASS_API_QT accordion_menu : public QWidget
{
public:

    SIGHT_UI_QT_API_QT accordion_menu(QWidget* _parent, Qt::Orientation _orientation);
    SIGHT_UI_QT_API_QT explicit accordion_menu(QToolBar* _tool_bar);
    SIGHT_UI_QT_API_QT ~accordion_menu() override = default;

    SIGHT_UI_QT_API_QT void fold();
    SIGHT_UI_QT_API_QT void unfold();
    SIGHT_UI_QT_API_QT void set_folded(bool _folded);
    SIGHT_UI_QT_API_QT void set_unfolded(bool _unfolded);
    SIGHT_UI_QT_API_QT void toggle_fold();
    [[nodiscard]] SIGHT_UI_QT_API_QT bool is_folded() const;

    SIGHT_UI_QT_API_QT void add_widget(QWidget* _w);
    SIGHT_UI_QT_API_QT void update();

    bool eventFilter(QObject* _o, QEvent* _e) override;

protected:

    void resizeEvent(QResizeEvent* _e) override;
    void childEvent(QChildEvent* _e) override;

private:

    [[nodiscard]] std::vector<QWidget*> children_widgets() const;

    Qt::Orientation m_orientation             = Qt::Horizontal;
    bool m_folded                             = true;
    QParallelAnimationGroup m_animation_group = {};
    QMetaObject::Connection m_first_button_connection;
    QPushButton* m_bracket = new QPushButton;
    QPixmap m_pixmap;
};

} // namespace sight::ui::qt::widget
