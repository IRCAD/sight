/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "list_view.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>

namespace sight::module::ui::qt
{

const core::com::signals::key_t list_view::ITEM_ADDED_SIG          = "itemAdded";
const core::com::signals::key_t list_view::ITEM_REMOVED_SIG        = "itemRemoved";
const core::com::signals::key_t list_view::ITEM_DOUBLE_CLICKED_SIG = "itemDoubleClicked";

const core::com::slots::key_t list_view::INSERT_ITEM_SLOT = "insertItem";
const core::com::slots::key_t list_view::REMOVE_ITEM_SLOT = "removeItem";

//------------------------------------------------------------------------------

list_view::list_view() noexcept
{
    new_signal<item_added_signal_t>(ITEM_ADDED_SIG);
    new_signal<item_removed_signal_t>(ITEM_REMOVED_SIG);
    new_signal<item_double_clicked_signal_t>(ITEM_DOUBLE_CLICKED_SIG);

    new_slot(INSERT_ITEM_SLOT, &list_view::insert_item, this);
    new_slot(REMOVE_ITEM_SLOT, &list_view::remove_item, this);
}

//------------------------------------------------------------------------------

list_view::~list_view() noexcept =
    default;

//------------------------------------------------------------------------------

bool list_view::eventFilter(QObject* _watched, QEvent* _event)
{
    // filter "del" key release event in order to delete the current selected item of the list.
    if(_watched == m_list_widget && _event->type() == QEvent::KeyRelease)
    {
        auto* event_key = static_cast<QKeyEvent*>(_event);
        if(event_key->key() == Qt::Key_Delete)
        {
            const QList<QListWidgetItem*>& list_item = m_list_widget->selectedItems();
            if(!list_item.empty())
            {
                this->remove_item(m_list_widget->row(list_item.first()));
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
void list_view::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------
void list_view::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    layout->setObjectName("list_viewLayout");
    m_list_widget = new(QListWidget);
    m_list_widget->setObjectName("list_viewWidget");
    m_list_widget->installEventFilter(this);
    layout->addWidget(m_list_widget);

    connect(m_list_widget, &QListWidget::itemDoubleClicked, this, &self_t::on_item_double_clicked);

    qt_container->set_layout(layout);
}

//------------------------------------------------------------------------------

void list_view::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void list_view::updating()
{
}

//------------------------------------------------------------------------------
void list_view::insert_item(int _index, std::string _value)
{
    // insert item at the index position of the list
    auto* new_item = new QListWidgetItem(QString::fromStdString(_value));
    m_list_widget->insertItem(_index, new_item);

    // notify
    this->signal<item_added_signal_t>(ITEM_ADDED_SIG)->async_emit(_index);
}

//------------------------------------------------------------------------------
void list_view::remove_item(int _index)
{
    // remove item at index position
    delete m_list_widget->takeItem(_index);

    // notify
    this->signal<item_removed_signal_t>(ITEM_REMOVED_SIG)->async_emit(_index);
}

//------------------------------------------------------------------------------

void list_view::on_item_double_clicked(QListWidgetItem* _item)
{
    const int index = m_list_widget->row(_item);
    this->signal<item_double_clicked_signal_t>(ITEM_DOUBLE_CLICKED_SIG)->async_emit(index);
}

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
