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

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>

namespace sight::ui::qt::widget
{

/**
 * @brief This class is simple collapsable section widget.
 *
 * It's possible to set a layout that contains all widgets that we want to add in collapsible section.
 * Then we can fold and unfold the section.
 *
 */
class UI_QT_CLASS_API_QT collapsible_section : public QWidget
{
Q_OBJECT

public:

    static const int DEFAULT_DURATION = 0;

    /**
     * @brief collapsible_section widget constructor.
     *
     * @param _title : the title to show next to QToolButton.
     * @param _animation_duration : the animation duration when the section fold/unfold.
     * @param _parent : the parent widget that will contain collapsible widget.
     */
    UI_QT_API_QT collapsible_section(
        const QString& _title         = "",
        const int _animation_duration = DEFAULT_DURATION,
        QWidget* _parent              = 0
    );

    /// Set layout inside the collapsible section with the desired content.
    UI_QT_API_QT void set_content_layout(QLayout* _content_layout);

    /// Set title next to the QToolButton.
    UI_QT_API_QT void set_title(QString _title);

    /**
     * @brief Update the height according to the contained data. If `_changing_content_while_unfolded`, the section
     * needs to toggle animation for update correctly the height.
     *
     * @param _changing_content_while_unfolded : If the content changes dynamically while the collapsible section is
     * unfolded.
     */
    UI_QT_API_QT void update_heights(bool _changing_content_while_unfolded);

public Q_SLOTS:

    /// Trigger the unfold/fold on the collapsible section
    void toggle(bool _expanded);

private:

    QGridLayout* m_main_layout                  = new QGridLayout(this);
    QToolButton* m_toggle_button                = new QToolButton(this);
    QFrame* m_header_line                       = new QFrame(this);
    QParallelAnimationGroup* m_toggle_animation = new QParallelAnimationGroup(this);
    QScrollArea* m_content_area                 = new QScrollArea(this);
    int m_animation_duration;
    int m_collapsed_height = 0;
    bool m_is_expanded     = false;

    // Delete all widgets in layout recursively
    void clean_layout_recursively(QLayout* _layout);
};

} // namespace sight::ui::qt::widget.
