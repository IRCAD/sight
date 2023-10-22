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

#include "ui/qt/widget/qfw_toolbox.hpp"

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

QfwToolBox::QfwToolBox(QWidget* _parent) :
    QFrame(_parent),
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
auto QfwToolBox::page(TOOLBOX& _toolbox, QWidget* _widget)
{
    using return_t = decltype(&*_toolbox.pageList.begin());
    if(_widget == nullptr)
    {
        return return_t(nullptr);
    }

    for(auto i = _toolbox.pageList.begin() ; i != _toolbox.pageList.end() ; ++i)
    {
        if(i->widget == _widget)
        {
            return &*i;
        }
    }

    return return_t(nullptr);
}

//------------------------------------------------------------------------------

const ui::qt::widget::Page* QfwToolBox::page(QWidget* _widget) const
{
    return page<>(*this, _widget);
}

//------------------------------------------------------------------------------

ui::qt::widget::Page* QfwToolBox::page(QWidget* _widget)
{
    return page<>(*this, _widget);
}

//-----------------------------------------------------------------------------

ui::qt::widget::Page* QfwToolBox::page(int _index)
{
    if(_index >= 0 && _index < pageList.size())
    {
        return &pageList[_index];
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

const ui::qt::widget::Page* QfwToolBox::page(int _index) const
{
    if(_index >= 0 && _index < pageList.size())
    {
        return &pageList.at(_index);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void QfwToolBox::collapseItem(int _index)
{
    Page* page = this->page(_index);
    if(page != nullptr)
    {
        page->sv->setVisible(false);
        page->button->setChecked(false);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::expandItem(int _index)
{
    Page* page = this->page(_index);
    if(page != nullptr)
    {
        page->sv->setVisible(true);
        page->button->setChecked(true);
    }
}

//-----------------------------------------------------------------------------

int QfwToolBox::addItem(QWidget* _item, const QString& _text)
{
    return insertItem(-1, _item, _text);
}

//-----------------------------------------------------------------------------

int QfwToolBox::insertItem(int _index, QWidget* _widget, const QString& _text)
{
    if(_widget == nullptr)
    {
        return -1;
    }

    connect(_widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));

    Page c {};
    c.widget = _widget;
    c.button = new QPushButton(this);
    c.button->setObjectName("QfwToolBoxButton");
    c.button->setBackgroundRole(QPalette::Window);
    c.button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    c.button->setFocusPolicy(Qt::NoFocus);
    c.button->setCheckable(true);
    QPixmap pix_open(branch_open.data());
    QPixmap pix_close(branch_closed.data());
    QIcon b_icon;
    b_icon.addPixmap(pix_close, QIcon::Normal, QIcon::Off);
    b_icon.addPixmap(pix_open, QIcon::Active, QIcon::On);
    c.setIcon(b_icon);
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
    auto* vertical_layout = new QVBoxLayout(c.sv);
    vertical_layout->setMargin(0);
    vertical_layout->addWidget(_widget);
    c.sv->hide();
    c.sv->setFrameStyle(QFrame::NoFrame);

    c.setText(_text);

    if(_index < 0 || _index >= (int) this->pageList.count())
    {
        _index = this->pageList.count();
        this->pageList.append(c);
        this->layout->addWidget(c.button);
        this->layout->addWidget(c.sv);
    }
    else
    {
        this->pageList.insert(_index, c);
        this->relayout();
    }

    c.button->show();
    return _index;
}

//-----------------------------------------------------------------------------

void QfwToolBox::buttonToggled(bool _checked)
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
    page->sv->setVisible(_checked);
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

void QfwToolBox::widgetDestroyed(QObject* _object)
{
    auto* p = dynamic_cast<QWidget*>(_object);
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

void QfwToolBox::removeItem(int _index)
{
    if(QWidget* w = widget(_index))
    {
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
        w->setParent(this);
        this->widgetDestroyed(w);
    }
}

//-----------------------------------------------------------------------------

QWidget* QfwToolBox::widget(int _index) const
{
    if(_index < 0 || _index >= (int) this->pageList.size())
    {
        return nullptr;
    }

    return this->pageList.at(_index).widget;
}

//-----------------------------------------------------------------------------

int QfwToolBox::indexOf(QWidget* _widget) const
{
    const Page* c = (_widget != nullptr ? this->page(_widget) : nullptr);
    return c != nullptr ? this->pageList.indexOf(*c) : -1;
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemEnabled(int _index, bool _enabled)
{
    Page* c = this->page(_index);
    if(c == nullptr)
    {
        return;
    }

    c->button->setEnabled(_enabled);
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemText(int _index, const QString& _text)
{
    Page* c = this->page(_index);
    if(c != nullptr)
    {
        c->setText(_text);
    }
}

//-----------------------------------------------------------------------------

void QfwToolBox::setItemToolTip(int _index, const QString& _tool_tip)
{
    Page* c = this->page(_index);
    if(c != nullptr)
    {
        c->setToolTip(_tool_tip);
    }
}

//-----------------------------------------------------------------------------

bool QfwToolBox::isItemEnabled(int _index) const
{
    const Page* c = this->page(_index);
    return (c != nullptr) && c->button->isEnabled();
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemText(int _index) const
{
    const Page* c = this->page(_index);
    return c != nullptr ? c->text() : QString();
}

//-----------------------------------------------------------------------------

QString QfwToolBox::itemToolTip(int _index) const
{
    const Page* c = this->page(_index);
    return c != nullptr ? c->toolTip() : QString();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::widget
