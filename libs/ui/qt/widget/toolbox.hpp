/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

/*
 * Based on (and also inspired by) QToolbox: http://doc.qt.nokia.com/4.7/qtoolbox.html
 */

#pragma once

#include <sight/ui/qt/config.hpp>

#include <QFormLayout>
#include <QObject>
#include <QPushButton>
#include <QScrollArea>
#include <QString>

namespace sight::ui::qt::widget
{

class page
{
public:

    //------------------------------------------------------------------------------

    void set_text(const QString& _text) const
    {
        button->setText(_text);
    }

    //------------------------------------------------------------------------------

    void set_icon(const QIcon& _is) const
    {
        button->setIcon(_is);
    }

    //------------------------------------------------------------------------------

    void set_tool_tip(const QString& _tip) const
    {
        button->setToolTip(_tip);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QString tool_tip() const
    {
        return button->toolTip();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QString text() const
    {
        return button->text();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QIcon icon() const
    {
        return button->icon();
    }

    //------------------------------------------------------------------------------

    inline bool operator==(const page& _other) const
    {
        return widget == _other.widget;
    }

    QPushButton* button;
    QFrame* sv;
    QWidget* widget;
};

class SIGHT_UI_QT_CLASS_API_QT toolbox : public QFrame
{
Q_OBJECT

public:

    SIGHT_UI_QT_API_QT toolbox(QWidget* _parent = nullptr);
    SIGHT_UI_QT_API_QT ~toolbox() override;

    SIGHT_UI_QT_API_QT int add_item(QWidget* _widget, const QString& _text);
    SIGHT_UI_QT_API_QT int insert_item(int _index, QWidget* _widget, const QString& _text);

    SIGHT_UI_QT_API_QT void remove_item(int _index);

    SIGHT_UI_QT_API_QT void set_item_enabled(int _index, bool _enabled);
    [[nodiscard]] SIGHT_UI_QT_API_QT bool is_item_enabled(int _index) const;

    SIGHT_UI_QT_API_QT void set_item_text(int _index, const QString& _text);
    [[nodiscard]] SIGHT_UI_QT_API_QT QString item_text(int _index) const;

    SIGHT_UI_QT_API_QT void set_item_tool_tip(int _index, const QString& _tool_tip);
    [[nodiscard]] SIGHT_UI_QT_API_QT QString item_tool_tip(int _index) const;

    [[nodiscard]] SIGHT_UI_QT_API_QT QWidget* widget(int _index) const;

    SIGHT_UI_QT_API_QT int index_of(QWidget* _widget) const;
    [[nodiscard]] SIGHT_UI_QT_API_QT int count() const;

    SIGHT_UI_QT_API_QT void collapse_item(int _index);
    SIGHT_UI_QT_API_QT void expand_item(int _index);

protected:

    using page_list = QList<ui::qt::widget::page>;

private Q_SLOTS:

    void button_toggled(bool _checked);
    void widget_destroyed(QObject* /*object*/);

private:

    template<typename TOOLBOX>
    static auto page(TOOLBOX& _toolbox, QWidget* _widget);
    [[nodiscard]] const ui::qt::widget::page* page(QWidget* _widget) const;
    ui::qt::widget::page* page(QWidget* _widget);
    [[nodiscard]] const ui::qt::widget::page* page(int _index) const;
    ui::qt::widget::page* page(int _index);

    void relayout();

    page_list m_page_list;
    QFormLayout* m_layout;
};

} // namespace sight::ui::qt::widget
