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

    void setText(const QString& text) const
    {
        button->setText(text);
    }

    //------------------------------------------------------------------------------

    void setIcon(const QIcon& is) const
    {
        button->setIcon(is);
    }

    //------------------------------------------------------------------------------

    void setToolTip(const QString& tip) const
    {
        button->setToolTip(tip);
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

    inline bool operator==(const Page& other) const
    {
        return widget == other.widget;
    }

    QPushButton* button;
    QFrame* sv;
    QWidget* widget;
};

class UI_QT_CLASS_API_QT QfwToolBox : public QFrame
{
Q_OBJECT

public:

    UI_QT_API_QT QfwToolBox(QWidget* parent = nullptr);
    UI_QT_API_QT ~QfwToolBox() override;

    UI_QT_API_QT int addItem(QWidget* widget, const QString& text);
    UI_QT_API_QT int insertItem(int index, QWidget* widget, const QString& text);

    UI_QT_API_QT void removeItem(int index);

    UI_QT_API_QT void setItemEnabled(int index, bool enabled);
    [[nodiscard]] UI_QT_API_QT bool isItemEnabled(int index) const;

    UI_QT_API_QT void setItemText(int index, const QString& text);
    [[nodiscard]] UI_QT_API_QT QString itemText(int index) const;

    UI_QT_API_QT void setItemToolTip(int index, const QString& toolTip);
    [[nodiscard]] UI_QT_API_QT QString itemToolTip(int index) const;

    [[nodiscard]] UI_QT_API_QT QWidget* widget(int index) const;

    UI_QT_API_QT int indexOf(QWidget* widget) const;
    [[nodiscard]] UI_QT_API_QT int count() const;

    UI_QT_API_QT void collapseItem(int index);
    UI_QT_API_QT void expandItem(int index);

protected:

    typedef QList<ui::qt::widget::Page> PageList;

private Q_SLOTS:

    void buttonToggled(bool checked);
    void widgetDestroyed(QObject* /*object*/);

private:

    template<typename TOOLBOX>
    static auto page(TOOLBOX& toolbox, QWidget* widget);
    [[nodiscard]] const ui::qt::widget::Page* page(QWidget* widget) const;
    ui::qt::widget::Page* page(QWidget* widget);
    [[nodiscard]] const ui::qt::widget::Page* page(int index) const;
    ui::qt::widget::Page* page(int index);

    void relayout();

    PageList pageList;
    QFormLayout* layout;
};

} // namespace sight::ui::qt::widget
