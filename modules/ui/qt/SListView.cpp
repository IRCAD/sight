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

#include "SListView.hpp"

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

const core::com::signals::key_t SListView::ITEM_ADDED_SIG          = "itemAdded";
const core::com::signals::key_t SListView::ITEM_REMOVED_SIG        = "itemRemoved";
const core::com::signals::key_t SListView::ITEM_DOUBLE_CLICKED_SIG = "itemDoubleClicked";

const core::com::slots::key_t SListView::INSERT_ITEM_SLOT = "insertItem";
const core::com::slots::key_t SListView::REMOVE_ITEM_SLOT = "removeItem";

//------------------------------------------------------------------------------

SListView::SListView() noexcept
{
    new_signal<ItemAddedSignalType>(ITEM_ADDED_SIG);
    new_signal<ItemRemovedSignalType>(ITEM_REMOVED_SIG);
    new_signal<ItemDoubleClickedSignalType>(ITEM_DOUBLE_CLICKED_SIG);

    new_slot(INSERT_ITEM_SLOT, &SListView::insertItem, this);
    new_slot(REMOVE_ITEM_SLOT, &SListView::removeItem, this);
}

//------------------------------------------------------------------------------

SListView::~SListView() noexcept =
    default;

//------------------------------------------------------------------------------

bool SListView::eventFilter(QObject* watched, QEvent* event)
{
    // filter "del" key release event in order to delete the current selected item of the list.
    if(watched == m_listWidget && event->type() == QEvent::KeyRelease)
    {
        auto* eventKey = static_cast<QKeyEvent*>(event);
        if(eventKey->key() == Qt::Key_Delete)
        {
            const QList<QListWidgetItem*>& listItem = m_listWidget->selectedItems();
            if(!listItem.empty())
            {
                this->removeItem(m_listWidget->row(listItem.first()));
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
void SListView::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------
void SListView::starting()
{
    this->create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    QPointer<QHBoxLayout> layout = new QHBoxLayout();
    layout->setObjectName("SListViewLayout");
    m_listWidget = new(QListWidget);
    m_listWidget->setObjectName("SListViewWidget");
    m_listWidget->installEventFilter(this);
    layout->addWidget(m_listWidget);

    connect(
        m_listWidget,
        SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this,
        SLOT(onItemDoubleClicked(QListWidgetItem*))
    );

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SListView::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SListView::updating()
{
}

//------------------------------------------------------------------------------
void SListView::insertItem(int index, std::string value)
{
    // insert item at the index position of the list
    auto* newItem = new QListWidgetItem(QString::fromStdString(value));
    m_listWidget->insertItem(index, newItem);

    // notify
    this->signal<ItemAddedSignalType>(ITEM_ADDED_SIG)->async_emit(index);
}

//------------------------------------------------------------------------------
void SListView::removeItem(int index)
{
    // remove item at index position
    delete m_listWidget->takeItem(index);

    // notify
    this->signal<ItemRemovedSignalType>(ITEM_REMOVED_SIG)->async_emit(index);
}

//------------------------------------------------------------------------------

void SListView::onItemDoubleClicked(QListWidgetItem* item)
{
    const int index = m_listWidget->row(item);
    this->signal<ItemDoubleClickedSignalType>(ITEM_DOUBLE_CLICKED_SIG)->async_emit(index);
}

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
