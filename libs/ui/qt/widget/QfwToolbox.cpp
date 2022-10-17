/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/widget/QfwToolbox.hpp"

#include <QApplication>
#include <QFrame>
#include <QIcon>
#include <QPixmap>
#include <QStyleOption>
#include <QVBoxLayout>

#include <array>

namespace sight::ui::qt::widget
{

static std::array branch_closed {
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

static std::array branch_open {
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
= default;

//-----------------------------------------------------------------------------

QfwToolBox::QfwToolBox(QWidget* parent) :
    QFrame(parent),
    layout(new QFormLayout(this))
{
    this->layout->setMargin(0);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(1);
    setBackgroundRole(QPalette::Button);
}

//-----------------------------------------------------------------------------

template<typename TOOLBOX>
auto QfwToolBox::page(TOOLBOX& toolbox, QWidget* widget)
{
    using ReturnType = decltype(&*toolbox.pageList.begin());
    if(widget == nullptr)
    {
        return ReturnType(nullptr);
    }

    for(auto i = toolbox.pageList.begin() ; i != toolbox.pageList.end() ; ++i)
    {
        if(i->widget == widget)
        {
            return &*i;
        }
    }

    return ReturnType(nullptr);
}

//------------------------------------------------------------------------------

const ui::qt::widget::Page* QfwToolBox::page(QWidget* widget) const
{
    return page<>(*this, widget);
}

//------------------------------------------------------------------------------

ui::qt::widget::Page* QfwToolBox::page(QWidget* widget)
{
    return page<>(*this, widget);
}

//-----------------------------------------------------------------------------

ui::qt::widget::Page* QfwToolBox::page(int index)
{
    if(index >= 0 && index < pageList.size())
    {
        return &pageList[index];
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

const ui::qt::widget::Page* QfwToolBox::page(int index) const
{
    if(index >= 0 && index < pageList.size())
    {
        return &pageList.at(index);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void QfwToolBox::collapseItem(int index)
{
    Page* page = this->page(index);
    if(page != nullptr)
    {
        page->sv->setVisible(false);
        page->button->setChecked(false);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::expandItem(int index)
{
    Page* page = this->page(index);
    if(page != nullptr)
    {
        page->sv->setVisible(true);
        page->button->setChecked(true);
    }
}

//-----------------------------------------------------------------------------

int QfwToolBox::addItem(QWidget* item, const QString& text)
{
    return insertItem(-1, item, text);
}

//-----------------------------------------------------------------------------

int QfwToolBox::insertItem(int index, QWidget* widget, const QString& text)
{
    if(widget == nullptr)
    {
        return -1;
    }

    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));

    Page c {};
    c.widget = widget;
    c.button = new QPushButton(this);
    c.button->setObjectName("QfwToolBoxButton");
    c.button->setBackgroundRole(QPalette::Window);
    c.button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    c.button->setFocusPolicy(Qt::NoFocus);
    c.button->setCheckable(true);
    QPixmap pixOpen(branch_open.data());
    QPixmap pixClose(branch_closed.data());
    QIcon bIcon;
    bIcon.addPixmap(pixClose, QIcon::Normal, QIcon::Off);
    bIcon.addPixmap(pixOpen, QIcon::Active, QIcon::On);
    c.setIcon(bIcon);
    if(qApp->styleSheet().isEmpty())
    {
        QString style(
            "text-align: left;"
            "background-color: lightgray;"
            "border-style: solid;"
            "border-width: 1px;"
            "border-color: darkgray;"
            "height: 20px;"
        );
        c.button->setStyleSheet(style);
    }

    connect(c.button, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)));

    c.sv = new QFrame(this);
    auto* verticalLayout = new QVBoxLayout(c.sv);
    verticalLayout->setMargin(0);
    verticalLayout->addWidget(widget);
    c.sv->hide();
    c.sv->setFrameStyle(QFrame::NoFrame);

    c.setText(text);

    if(index < 0 || index >= (int) this->pageList.count())
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
    auto* tb      = qobject_cast<QPushButton*>(this->sender());
    QWidget* item = nullptr;
    for(PageList::ConstIterator i = pageList.constBegin() ; i != pageList.constEnd() ; ++i)
    {
        if((*i).button == tb)
        {
            item = (*i).widget;
            break;
        }
    }

    int index  = this->indexOf(item);
    Page* page = this->page(index);
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
    for(PageList::ConstIterator i = pageList.constBegin() ; i != pageList.constEnd() ; ++i)
    {
        layout->addWidget((*i).button);
        layout->addWidget((*i).sv);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::widgetDestroyed(QObject* object)
{
    auto* p = dynamic_cast<QWidget*>(object);
    Page* c = page(p);
    if((p == nullptr) || (c == nullptr))
    {
        return;
    }

    layout->removeWidget(c->sv);
    layout->removeWidget(c->button);
    c->sv->deleteLater();
    delete c->button;
    pageList.removeAll(*c);
}

//-----------------------------------------------------------------------------

void QfwToolBox::removeItem(int index)
{
    if(QWidget* w = widget(index))
    {
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
        w->setParent(this);
        this->widgetDestroyed(w);
    }
}

//-----------------------------------------------------------------------------

QWidget* QfwToolBox::widget(int index) const
{
    if(index < 0 || index >= (int) this->pageList.size())
    {
        return nullptr;
    }

    return this->pageList.at(index).widget;
}

//-----------------------------------------------------------------------------

int QfwToolBox::indexOf(QWidget* widget) const
{
    const Page* c = (widget != nullptr ? this->page(widget) : nullptr);
    return c != nullptr ? this->pageList.indexOf(*c) : -1;
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemEnabled(int index, bool enabled)
{
    Page* c = this->page(index);
    if(c == nullptr)
    {
        return;
    }

    c->button->setEnabled(enabled);
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemText(int index, const QString& text)
{
    Page* c = this->page(index);
    if(c != nullptr)
    {
        c->setText(text);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemToolTip(int index, const QString& toolTip)
{
    Page* c = this->page(index);
    if(c != nullptr)
    {
        c->setToolTip(toolTip);
    }
}

//-----------------------------------------------------------------------------

bool QfwToolBox::isItemEnabled(int index) const
{
    const Page* c = this->page(index);
    return (c != nullptr) && c->button->isEnabled();
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemText(int index) const
{
    const Page* c = this->page(index);
    return c != nullptr ? c->text() : QString();
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemToolTip(int index) const
{
    const Page* c = this->page(index);
    return c != nullptr ? c->toolTip() : QString();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::widget
