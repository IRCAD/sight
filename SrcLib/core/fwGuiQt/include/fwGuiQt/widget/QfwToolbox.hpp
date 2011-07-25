/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*
 * Based on (and also inspired by) QToolbox: http://doc.qt.nokia.com/4.7/qtoolbox.html
 */

#ifndef _FWGUIQT_WIDGET_QFWTOOLBOX_HPP
#define _FWGUIQT_WIDGET_QFWTOOLBOX_HPP

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
    void setText(const QString &text) { button->setText(text); }
    void setIcon(const QIcon &is) { button->setIcon(is); }
    void setToolTip(const QString &tip) { button->setToolTip(tip); }
    QString toolTip() const { return button->toolTip(); }
    QString text() const { return button->text(); }
    QIcon icon() const { return button->icon(); }

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

private slots:
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

#endif // _FWGUIQT_WIDGET_QFWTOOLBOX_HPP
