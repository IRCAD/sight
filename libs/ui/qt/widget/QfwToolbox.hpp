/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::ui::qt
{
namespace widget
{

class Page
{
public:
    //------------------------------------------------------------------------------

    void setText(const QString& text)
    {
        button->setText(text);
    }
    //------------------------------------------------------------------------------

    void setIcon(const QIcon& is)
    {
        button->setIcon(is);
    }
    //------------------------------------------------------------------------------

    void setToolTip(const QString& tip)
    {
        button->setToolTip(tip);
    }
    //------------------------------------------------------------------------------

    QString toolTip() const
    {
        return button->toolTip();
    }
    //------------------------------------------------------------------------------

    QString text() const
    {
        return button->text();
    }
    //------------------------------------------------------------------------------

    QIcon icon() const
    {
        return button->icon();
    }

    //------------------------------------------------------------------------------

    inline bool operator==(const Page& other) const
    {
        return widget == other.widget;
    }

    QPushButton* button;
    QFrame* sv;
    QWidget* widget;
};

class UI_QT_CLASS_API QfwToolBox : public QFrame
{
Q_OBJECT

public:
    UI_QT_API QfwToolBox(QWidget* parent = 0, Qt::WindowFlags f = 0);
    UI_QT_API virtual ~QfwToolBox();

    UI_QT_API int addItem(QWidget* widget, const QString& text);
    UI_QT_API int insertItem(int index, QWidget* widget, const QString& text);

    UI_QT_API void removeItem(int index);

    UI_QT_API void setItemEnabled(int index, bool enabled);
    UI_QT_API bool isItemEnabled(int index) const;

    UI_QT_API void setItemText(int index, const QString& text);
    UI_QT_API QString itemText(int index) const;

    UI_QT_API void setItemToolTip(int index, const QString& toolTip);
    UI_QT_API QString itemToolTip(int index) const;

    UI_QT_API QWidget* widget(int index) const;

    UI_QT_API int indexOf(QWidget* widget) const;
    UI_QT_API int count() const;

    UI_QT_API void collapseItem(int index);
    UI_QT_API void expandItem(int index);

protected:

    typedef QList< ui::qt::widget::Page > PageList;

private Q_SLOTS:
    void buttonToggled(bool checked);
    void widgetDestroyed(QObject*);

private:

    ui::qt::widget::Page* page(QWidget* widget) const;
    const ui::qt::widget::Page* page(int index) const;
    ui::qt::widget::Page* page(int index);

    void relayout();

    PageList pageList;
    QFormLayout* layout;
};

} // namespace widget
} // namespace sight::ui::qt
