/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ui/qt/dialog/selector.hpp"

#include <core/base.hpp>

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

void selector::set_choices_preset(choices_preset_t _choices)
{
    this->m_choices = _choices;
}

//------------------------------------------------------------------------------

void selector::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

void selector::set_multiple(bool _multiple)
{
    this->m_multiple = _multiple;
}

//------------------------------------------------------------------------------

selector::selections_t selector::show()
{
    QWidget* parent = qApp->activeWindow();

    auto* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString(m_title));
    dialog->setObjectName("selector");

    auto* selection_list = new QListWidget(dialog);
    selection_list->setObjectName("selectionList");
    int selected_item = 0;
    for(const auto& choice : m_choices)
    {
        int item_index = 0;
        if(m_multiple)
        {
            auto* item = new QListWidgetItem(QString::fromStdString(choice.first), selection_list);
            item->setCheckState((choice.second ? Qt::Checked : Qt::Unchecked));
            selection_list->addItem(item);
        }
        else
        {
            selection_list->addItem(QString::fromStdString(choice.first));
        }

        selected_item = choice.second ? item_index : selected_item;
        ++item_index;
    }

    QListWidgetItem* first_item = selection_list->item(selected_item);
    selection_list->setCurrentItem(first_item);

    auto* ok_button = new QPushButton(QObject::tr("Ok"));
    ok_button->setObjectName("Ok");
    auto* cancel_button = new QPushButton(QObject::tr("Cancel"));
    cancel_button->setObjectName("Cancel");

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(ok_button);
    h_layout->addWidget(cancel_button);

    for(auto* custom_button : m_customButtons)
    {
        h_layout->addWidget(custom_button);
        QObject::connect(custom_button, SIGNAL(clicked()), dialog, SLOT(reject()));
    }

    auto* v_layout = new QVBoxLayout();
    if(!m_message.empty())
    {
        auto* msg_text = new QLabel(QString::fromStdString(m_message), dialog);
        v_layout->addWidget(msg_text);
    }

    v_layout->addWidget(selection_list);
    v_layout->addLayout(h_layout);

    dialog->setLayout(v_layout);
    QObject::connect(ok_button, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancel_button, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selection_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), dialog, SLOT(accept()));

    selections_t selections;
    if(dialog->exec() != 0)
    {
        int index_item = 0;
        if(m_multiple)
        {
            for(const auto& selection : m_choices)
            {
                if(selection_list->item(index_item)->checkState() == Qt::Checked)
                {
                    selections.push_back(selection.first);
                    index_item++;
                }
            }
        }
        else
        {
            selections.push_back(selection_list->currentItem()->text().toStdString());
        }
    }

    return selections;
}

//------------------------------------------------------------------------------

void selector::setMessage(const std::string& _msg)
{
    m_message = _msg;
}

//------------------------------------------------------------------------------

void selector::addCustomButton(const std::string& _label, std::function<void()> _clicked_fn)
{
    auto* button = new QPushButton(QString::fromStdString(_label));
    m_customButtons.push_back(button);
    QObject::connect(button, &QPushButton::clicked, _clicked_fn);
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
