/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QFrame>
#include <QPixmap>
#include <QIcon>
#include <QVBoxLayout>

#include "fwGuiQt/widget/QfwToolbox.hpp"

namespace fwGuiQt
{
namespace widget
{

static const char *branch_closed[] = {
"8 17 2 1",
/* colors */
"- c #000000",
"a c None",
/* pixels */
"aaaaaaaa",
"-aaaaaaa",
"--aaaaaa",
"---aaaaa",
"----aaaa",
"-----aaa",
"------aa",
"-------a",
"--------",
"-------a",
"------aa",
"-----aaa",
"----aaaa",
"---aaaaa",
"--aaaaaa",
"-aaaaaaa",
"aaaaaaaa"
};

static const char *branch_open[] = {
"11 6 2 1",
/* colors */
"- c #000000",
"a c None",
/* pixels */
"-----------",
"a---------a",
"aa-------aa",
"aaa-----aaa",
"aaaa---aaaa",
"aaaaa-aaaaa"
};

QfwToolBox::~QfwToolBox()
{}

//-----------------------------------------------------------------------------

QfwToolBox::QfwToolBox(QWidget *parent, Qt::WindowFlags f) :  QFrame(parent, f)
{
    this->layout = new QFormLayout(this);
    this->layout->setMargin(0);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(1);
    setBackgroundRole(QPalette::Button);
}

//-----------------------------------------------------------------------------

::fwGuiQt::widget::Page *QfwToolBox::page(QWidget *widget) const
{
    if (!widget)
        return 0;

    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
        if ((*i).widget == widget)
            return (Page*) &(*i);
    return 0;
}

//-----------------------------------------------------------------------------

::fwGuiQt::widget::Page *QfwToolBox::page(int index)
{
    if (index >= 0 && index < pageList.size())
        return &pageList[index];
    return 0;
}

//-----------------------------------------------------------------------------

const ::fwGuiQt::widget::Page *QfwToolBox::page(int index) const
{
    if (index >= 0 && index < pageList.size())
        return &pageList.at(index);
    return 0;
}

//-----------------------------------------------------------------------------

void QfwToolBox::collapseItem(int index)
{
    Page *page = this->page(index);
    if(page)
    {
        page->sv->setVisible(false);
        page->button->setChecked(false);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::expandItem(int index)
{
    Page *page = this->page(index);
    if(page)
    {
        page->sv->setVisible(true);
        page->button->setChecked(true);
    }
}

//-----------------------------------------------------------------------------

int QfwToolBox::addItem(QWidget *item, const QString &text)
{
    return insertItem(-1, item, text);
}

//-----------------------------------------------------------------------------

int QfwToolBox::insertItem(int index, QWidget *widget, const QString &text)
{
    if (!widget)
        return -1;

    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));

    Page c;
    c.widget = widget;
    c.button = new QPushButton(this);
    c.button->setBackgroundRole(QPalette::Window);
    c.button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    c.button->setFocusPolicy(Qt::NoFocus);
    c.button->setCheckable(true);
    QString style(
            "text-align: left;"
            "background-color: lightgray;"
            "border-style: solid;"
            "border-width: 1px;"
            "border-color: darkgray;"
            "height: 20px;"
            );
    QPixmap pixOpen(branch_open);
    QPixmap pixClose(branch_closed);
    QIcon bIcon;
    bIcon.addPixmap( pixClose, QIcon::Normal, QIcon::Off );
    bIcon.addPixmap( pixOpen, QIcon::Active, QIcon::On );
    c.setIcon(bIcon);
    c.button->setStyleSheet(style);
    connect(c.button, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)));

    c.sv = new QFrame(this);
    QVBoxLayout* verticalLayout = new QVBoxLayout(c.sv);
    verticalLayout->setMargin(0);
    verticalLayout->addWidget(widget);
    c.sv->hide();
    c.sv->setFrameStyle(QFrame::NoFrame);

    c.setText(text);

    if (index < 0 || index >= (int)this->pageList.count())
    {
        index = this->pageList.count();
        this->pageList.append(c);
        this->layout->addWidget(c.button);
        this->layout->addWidget(c.sv);
    }
    else
    {
        this->pageList.insert(index, c);
        this->relayout();
    }
    c.button->show();
    return index;
}

//-----------------------------------------------------------------------------

void QfwToolBox::buttonToggled(bool checked)
{
    QPushButton *tb = qobject_cast<QPushButton*>(this->sender());
    QWidget* item = 0;
    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
    {
        if ((*i).button == tb)
        {
            item = (*i).widget;
            break;
        }
    }
    int index = this->indexOf(item);
    Page *page = this->page(index);
    page->sv->setVisible(checked);
}

//-----------------------------------------------------------------------------

int QfwToolBox::count() const
{
    return this->pageList.count();
}

//-----------------------------------------------------------------------------

void QfwToolBox::relayout()
{
    delete layout;
    layout = new QFormLayout(this);
    layout->setMargin(0);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(1);
    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
    {
        layout->addWidget((*i).button);
        layout->addWidget((*i).sv);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::widgetDestroyed(QObject *object)
{
    QWidget *p = (QWidget*)object;
    Page *c = page(p);
    if (!p || !c)
        return;

    layout->removeWidget(c->sv);
    layout->removeWidget(c->button);
    c->sv->deleteLater();
    delete c->button;
    pageList.removeAll(*c);
}

//-----------------------------------------------------------------------------

void QfwToolBox::removeItem(int index)
{
    if (QWidget *w = widget(index))
    {
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
        w->setParent(this);
        this->widgetDestroyed(w);
    }
}

//-----------------------------------------------------------------------------

QWidget *QfwToolBox::widget(int index) const
{
    if (index < 0 || index >= (int) this->pageList.size())
        return 0;
    return this->pageList.at(index).widget;
}

//-----------------------------------------------------------------------------

int QfwToolBox::indexOf(QWidget *widget) const
{
    Page *c = (widget ? this->page(widget) : 0);
    return c ? this->pageList.indexOf(*c) : -1;
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemEnabled(int index, bool enabled)
{
    Page *c = this->page(index);
    if (!c)
        return;

    c->button->setEnabled(enabled);
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemText(int index, const QString &text)
{
    Page *c = this->page(index);
    if (c)
        c->setText(text);
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemToolTip(int index, const QString &toolTip)
{
    Page *c = this->page(index);
    if (c)
        c->setToolTip(toolTip);
}

//-----------------------------------------------------------------------------

bool QfwToolBox::isItemEnabled(int index) const
{
    const Page *c = this->page(index);
    return c && c->button->isEnabled();
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemText(int index) const
{
    const Page *c = this->page(index);
    return (c ? c->text() : QString());
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemToolTip(int index) const
{
    const Page *c = this->page(index);
    return (c ? c->toolTip() : QString());
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace fwGuiQt
