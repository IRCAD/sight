/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/config.hpp"

#include <QFormLayout>
#include <QObject>
#include <QPushButton>
#include <QScrollArea>
#include <QString>

namespace sight::ui::qt::widget
{

class Page
{
public:

    //------------------------------------------------------------------------------

    void setText(const QString& _text) const
    {
        button->setText(_text);
    }

    //------------------------------------------------------------------------------

    void setIcon(const QIcon& _is) const
    {
        button->setIcon(_is);
    }

    //------------------------------------------------------------------------------

    void setToolTip(const QString& _tip) const
    {
        button->setToolTip(_tip);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QString toolTip() const
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

    inline bool operator==(const Page& _other) const
    {
        return widget == _other.widget;
    }

    QPushButton* button;
    QFrame* sv;
    QWidget* widget;
};

class UI_QT_CLASS_API_QT QfwToolBox : public QFrame
{
Q_OBJECT

public:

    UI_QT_API_QT QfwToolBox(QWidget* _parent = nullptr);
    UI_QT_API_QT ~QfwToolBox() override;

    UI_QT_API_QT int addItem(QWidget* _widget, const QString& _text);
    UI_QT_API_QT int insertItem(int _index, QWidget* _widget, const QString& _text);

    UI_QT_API_QT void removeItem(int _index);

    UI_QT_API_QT void setItemEnabled(int _index, bool _enabled);
    [[nodiscard]] UI_QT_API_QT bool isItemEnabled(int _index) const;

    UI_QT_API_QT void setItemText(int _index, const QString& _text);
    [[nodiscard]] UI_QT_API_QT QString itemText(int _index) const;

    UI_QT_API_QT void setItemToolTip(int _index, const QString& _tool_tip);
    [[nodiscard]] UI_QT_API_QT QString itemToolTip(int _index) const;

    [[nodiscard]] UI_QT_API_QT QWidget* widget(int _index) const;

    UI_QT_API_QT int indexOf(QWidget* _widget) const;
    [[nodiscard]] UI_QT_API_QT int count() const;

    UI_QT_API_QT void collapseItem(int _index);
    UI_QT_API_QT void expandItem(int _index);

protected:

    typedef QList<ui::qt::widget::Page> PageList;

private Q_SLOTS:

    void buttonToggled(bool _checked);
    void widgetDestroyed(QObject* /*object*/);

private:

    template<typename TOOLBOX>
    static auto page(TOOLBOX& _toolbox, QWidget* _widget);
    [[nodiscard]] const ui::qt::widget::Page* page(QWidget* _widget) const;
    ui::qt::widget::Page* page(QWidget* _widget);
    [[nodiscard]] const ui::qt::widget::Page* page(int _index) const;
    ui::qt::widget::Page* page(int _index);

    void relayout();

    PageList pageList;
    QFormLayout* layout;
};

} // namespace sight::ui::qt::widget
