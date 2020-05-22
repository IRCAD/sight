/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWGUIQT_WIDGET_QFWTOOLBOX_HPP__
#define __FWGUIQT_WIDGET_QFWTOOLBOX_HPP__

#include <QObject>
#include <QScrollArea>
#include <QString>
#include <QFormLayout>
#include <QPushButton>

#include "fwGuiQt/config.hpp"

namespace fwGuiQt
{
namespace widget
{


class Page
{
public:
    void setText(const QString &text)
    {
        button->setText(text);
    }
    void setIcon(const QIcon &is)
    {
        button->setIcon(is);
    }
    void setToolTip(const QString &tip)
    {
        button->setToolTip(tip);
    }
    QString toolTip() const
    {
        return button->toolTip();
    }
    QString text() const
    {
        return button->text();
    }
    QIcon icon() const
    {
        return button->icon();
    }

    inline bool operator==(const Page& other) const
    {
        return widget == other.widget;
    }

    QPushButton *button;
    QFrame* sv;
    QWidget *widget;
};

class FWGUIQT_CLASS_API QfwToolBox : public QFrame
{
Q_OBJECT

public:
    FWGUIQT_API QfwToolBox(QWidget *parent = 0, Qt::WindowFlags f = 0);
    FWGUIQT_API virtual ~QfwToolBox();

    FWGUIQT_API int addItem(QWidget *widget, const QString &text);
    FWGUIQT_API int insertItem(int index, QWidget *widget, const QString &text);

    FWGUIQT_API void removeItem(int index);

    FWGUIQT_API void setItemEnabled(int index, bool enabled);
    FWGUIQT_API bool isItemEnabled(int index) const;

    FWGUIQT_API void setItemText(int index, const QString &text);
    FWGUIQT_API QString itemText(int index) const;

    FWGUIQT_API void setItemToolTip(int index, const QString &toolTip);
    FWGUIQT_API QString itemToolTip(int index) const;

    FWGUIQT_API QWidget *widget(int index) const;

    FWGUIQT_API int indexOf(QWidget *widget) const;
    FWGUIQT_API int count() const;

    FWGUIQT_API void collapseItem(int index);
    FWGUIQT_API void expandItem(int index);

protected:

    typedef QList< ::fwGuiQt::widget::Page > PageList;

private Q_SLOTS:
    void buttonToggled(bool checked);
    void widgetDestroyed(QObject*);

private:

    ::fwGuiQt::widget::Page* page(QWidget *widget) const;
    const ::fwGuiQt::widget::Page* page(int index) const;
    ::fwGuiQt::widget::Page* page(int index);

    void relayout();

    PageList pageList;
    QFormLayout* layout;
};


} // namespace widget
} // namespace fwGuiQt

#endif // __FWGUIQT_WIDGET_QFWTOOLBOX_HPP__
