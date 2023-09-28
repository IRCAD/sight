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

    auto* selectionList = new QListWidget(dialog);
    selectionList->setObjectName("selectionList");
    int selected_item = 0;
    for(const auto& choice : m_choices)
    {
        int item_index = 0;
        if(m_multiple)
        {
            auto* item = new QListWidgetItem(QString::fromStdString(choice.first), selectionList);
            item->setCheckState((choice.second ? Qt::Checked : Qt::Unchecked));
            selectionList->addItem(item);
        }
        else
        {
            selectionList->addItem(QString::fromStdString(choice.first));
        }

        selected_item = choice.second ? item_index : selected_item;
        ++item_index;
    }

    QListWidgetItem* firstItem = selectionList->item(selected_item);
    selectionList->setCurrentItem(firstItem);

    auto* okButton = new QPushButton(QObject::tr("Ok"));
    okButton->setObjectName("Ok");
    auto* cancelButton = new QPushButton(QObject::tr("Cancel"));
    cancelButton->setObjectName("Cancel");

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(okButton);
    h_layout->addWidget(cancelButton);

    for(auto* customButton : m_customButtons)
    {
        h_layout->addWidget(customButton);
        QObject::connect(customButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    }

    auto* vLayout = new QVBoxLayout();
    if(!m_message.empty())
    {
        auto* msgText = new QLabel(QString::fromStdString(m_message), dialog);
        vLayout->addWidget(msgText);
    }

    vLayout->addWidget(selectionList);
    vLayout->addLayout(h_layout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), dialog, SLOT(accept()));

    selections_t selections;
    if(dialog->exec() != 0)
    {
        int indexItem = 0;
        if(m_multiple)
        {
            for(const auto& selection : m_choices)
            {
                if(selectionList->item(indexItem)->checkState() == Qt::Checked)
                {
                    selections.push_back(selection.first);
                    indexItem++;
                }
            }
        }
        else
        {
            selections.push_back(selectionList->currentItem()->text().toStdString());
        }
    }

    return selections;
}

//------------------------------------------------------------------------------

void selector::setMessage(const std::string& msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

void selector::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    auto* button = new QPushButton(QString::fromStdString(label));
    m_customButtons.push_back(button);
    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
