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

#pragma once

#include "ui/qt/config.hpp"

#include <QGraphicsView>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include <QToolBar>

namespace sight::ui::qt::widget
{

class UI_QT_CLASS_API_QT AccordionMenu : public QWidget
{
public:

    UI_QT_API_QT AccordionMenu(QWidget* parent, Qt::Orientation orientation);
    UI_QT_API_QT explicit AccordionMenu(QToolBar* toolBar);
    UI_QT_API_QT ~AccordionMenu() override = default;

    UI_QT_API_QT void fold();
    UI_QT_API_QT void unfold();
    UI_QT_API_QT void setFolded(bool folded);
    UI_QT_API_QT void setUnfolded(bool unfolded);
    UI_QT_API_QT void toggleFold();
    [[nodiscard]] UI_QT_API_QT bool isFolded() const;

    UI_QT_API_QT void addWidget(QWidget* w);
    UI_QT_API_QT void update();

    bool eventFilter(QObject* o, QEvent* e) override;

protected:

    void resizeEvent(QResizeEvent* e) override;
    void childEvent(QChildEvent* e) override;

private:

    [[nodiscard]] std::vector<QWidget*> childrenWidgets() const;

    Qt::Orientation m_orientation             = Qt::Horizontal;
    bool m_folded                             = true;
    QParallelAnimationGroup* m_animationGroup = new QParallelAnimationGroup;
    QMetaObject::Connection m_firstButtonConnection;
    QPushButton* m_bracket = new QPushButton;
    QPixmap m_pixmap;
};

} // namespace sight::ui::qt::widget
