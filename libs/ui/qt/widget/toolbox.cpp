/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/qt/widget/toolbox.hpp"

#include <core/spy_log.hpp>

#include <QApplication>
#include <QFrame>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QStyleOption>
#include <QVBoxLayout>

namespace sight::ui::qt::widget
{

toolbox::~toolbox()
= default;

//-----------------------------------------------------------------------------

toolbox::toolbox(QWidget* _parent, QColor _icon_color) :
    QFrame(_parent),
    m_icon_color(_icon_color),
    m_layout(new QFormLayout(this))
{
    this->m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_layout->setHorizontalSpacing(0);
    m_layout->setVerticalSpacing(1);
    setBackgroundRole(QPalette::Button);
}

//-----------------------------------------------------------------------------

template<typename TOOLBOX>
auto toolbox::page(TOOLBOX& _toolbox, QWidget* _widget)
{
    using return_t = decltype(&*_toolbox.m_page_list.begin());
    if(_widget == nullptr)
    {
        return return_t(nullptr);
    }

    for(auto i = _toolbox.m_page_list.begin() ; i != _toolbox.m_page_list.end() ; ++i)
    {
        if(i->widget == _widget)
        {
            return &*i;
        }
    }

    return return_t(nullptr);
}

//------------------------------------------------------------------------------

const ui::qt::widget::page* toolbox::page(QWidget* _widget) const
{
    return page<>(*this, _widget);
}

//------------------------------------------------------------------------------

ui::qt::widget::page* toolbox::page(QWidget* _widget)
{
    return page<>(*this, _widget);
}

//-----------------------------------------------------------------------------

ui::qt::widget::page* toolbox::page(int _index)
{
    if(_index >= 0 && _index < m_page_list.size())
    {
        return &m_page_list[_index];
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

const ui::qt::widget::page* toolbox::page(int _index) const
{
    if(_index >= 0 && _index < m_page_list.size())
    {
        return &m_page_list.at(_index);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void toolbox::collapse_item(int _index)
{
    widget::page* page = this->page(_index);
    if(page != nullptr)
    {
        page->sv->setVisible(false);
        page->button->setChecked(false);
    }
}

//-----------------------------------------------------------------------------

void toolbox::expand_item(int _index)
{
    widget::page* page = this->page(_index);
    if(page != nullptr)
    {
        page->sv->setVisible(true);
        page->button->setChecked(true);
    }
}

//-----------------------------------------------------------------------------

int toolbox::add_item(QWidget* _item, const QString& _text)
{
    return insert_item(-1, _item, _text);
}

//-----------------------------------------------------------------------------

int toolbox::insert_item(int _index, QWidget* _widget, const QString& _text)
{
    if(_widget == nullptr)
    {
        return -1;
    }

    connect(_widget, &QWidget::destroyed, this, &toolbox::widget_destroyed);

    class page c
    {
    };
    c.widget = _widget;
    c.button = new QPushButton(this);
    c.button->setObjectName("QfwToolBoxButton");
    c.button->setProperty("class", "toolbox_button");
    c.button->setBackgroundRole(QPalette::Window);
    c.button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    c.button->setFocusPolicy(Qt::NoFocus);
    c.button->setCheckable(true);

    // Opened and closed icons are drawn manually.
    QPixmap pix_open(QSize(50, 80));
    pix_open.fill(Qt::transparent); // Make sure background is transparent.
    QPainter painter_open;
    painter_open.begin(&pix_open);
    QPolygonF triangle;
    triangle << QPointF(0, 15) << QPointF(50, 15) << QPointF(25, 60);
    QPainterPath path;
    path.addPolygon(triangle);
    painter_open.fillPath(path, m_icon_color);
    painter_open.end();

    QPixmap pix_close(QSize(50, 80));
    pix_close.fill(Qt::transparent);
    QPainter painter_close;
    painter_close.begin(&pix_close);
    triangle.clear();
    triangle << QPointF(0, 15) << QPointF(0, 65) << QPointF(50, 40);
    path.clear();
    path.addPolygon(triangle);
    painter_close.fillPath(path, m_icon_color);
    painter_close.end();

    QIcon b_icon;
    b_icon.addPixmap(pix_close, QIcon::Normal, QIcon::Off);
    b_icon.addPixmap(pix_open, QIcon::Active, QIcon::On);
    c.set_icon(b_icon);

    connect(c.button, &QPushButton::toggled, this, &toolbox::button_toggled);

    c.sv = new QFrame(this);
    auto* vertical_layout = new QVBoxLayout(c.sv);
    vertical_layout->setContentsMargins(0, 0, 0, 0);
    vertical_layout->addWidget(_widget);
    c.sv->hide();
    c.sv->setProperty("class", "toolbox_frame");
    c.sv->setFrameStyle(QFrame::NoFrame);

    c.set_text(_text);

    if(_index < 0 || _index >= this->m_page_list.count())
    {
        _index = int(this->m_page_list.count());
        this->m_page_list.append(c);
        this->m_layout->addWidget(c.button);
        this->m_layout->addWidget(c.sv);
    }
    else
    {
        this->m_page_list.insert(_index, c);
        this->relayout();
    }

    c.button->show();
    return _index;
}

//-----------------------------------------------------------------------------

void toolbox::button_toggled(bool _checked)
{
    auto* tb      = qobject_cast<QPushButton*>(this->sender());
    QWidget* item = nullptr;
    for(const auto& page : m_page_list)
    {
        if(page.button == tb)
        {
            item = page.widget;
            break;
        }
    }

    int index          = this->index_of(item);
    widget::page* page = this->page(index);
    page->sv->setVisible(_checked);
}

//-----------------------------------------------------------------------------

int toolbox::count() const
{
    return int(this->m_page_list.count());
}

//-----------------------------------------------------------------------------

void toolbox::relayout()
{
    delete m_layout;
    m_layout = new QFormLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_layout->setHorizontalSpacing(0);
    m_layout->setVerticalSpacing(1);
    for(const auto& page : m_page_list)
    {
        m_layout->addWidget(page.button);
        m_layout->addWidget(page.sv);
    }
}

//-----------------------------------------------------------------------------

void toolbox::widget_destroyed(QObject* _object)
{
    auto* p         = dynamic_cast<QWidget*>(_object);
    widget::page* c = page(p);
    if((p == nullptr) || (c == nullptr))
    {
        return;
    }

    m_layout->removeWidget(c->sv);
    m_layout->removeWidget(c->button);
    c->sv->deleteLater();
    delete c->button;
    m_page_list.removeAll(*c);
}

//-----------------------------------------------------------------------------

void toolbox::remove_item(int _index)
{
    if(QWidget* w = widget(_index))
    {
        disconnect(w, &QWidget::destroyed, this, &toolbox::widget_destroyed);
        w->setParent(this);
        this->widget_destroyed(w);
    }
}

//-----------------------------------------------------------------------------

QWidget* toolbox::widget(int _index) const
{
    if(_index < 0 || _index >= this->m_page_list.size())
    {
        return nullptr;
    }

    return this->m_page_list.at(_index).widget;
}

//-----------------------------------------------------------------------------

int toolbox::index_of(QWidget* _widget) const
{
    const widget::page* c = (_widget != nullptr ? this->page(_widget) : nullptr);
    return c != nullptr ? int(this->m_page_list.indexOf(*c)) : -1;
}

//-----------------------------------------------------------------------------

void toolbox::set_item_enabled(int _index, bool _enabled)
{
    widget::page* c = this->page(_index);
    if(c == nullptr)
    {
        return;
    }

    c->button->setEnabled(_enabled);
}

//-----------------------------------------------------------------------------

void toolbox::set_item_text(int _index, const QString& _text)
{
    widget::page* c = this->page(_index);
    if(c != nullptr)
    {
        c->set_text(_text);
    }
}

//-----------------------------------------------------------------------------

void toolbox::set_item_tool_tip(int _index, const QString& _tool_tip)
{
    widget::page* c = this->page(_index);
    if(c != nullptr)
    {
        c->set_tool_tip(_tool_tip);
    }
}

//-----------------------------------------------------------------------------

bool toolbox::is_item_enabled(int _index) const
{
    const widget::page* c = this->page(_index);
    return (c != nullptr) && c->button->isEnabled();
}

//-----------------------------------------------------------------------------

QString toolbox::item_text(int _index) const
{
    const widget::page* c = this->page(_index);
    return c != nullptr ? c->text() : QString();
}

//-----------------------------------------------------------------------------

QString toolbox::item_tool_tip(int _index) const
{
    const widget::page* c = this->page(_index);
    return c != nullptr ? c->tool_tip() : QString();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::widget
