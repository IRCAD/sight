/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/SelectorDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

fwGuiRegisterMacro( ::fwGuiQml::dialog::SelectorDialog, ::fwGui::dialog::ISelectorDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

SelectorDialog::SelectorDialog(::fwGui::GuiBaseObject::Key key) :
    m_message(""),
    m_title("")
{
}

//------------------------------------------------------------------------------

SelectorDialog::~SelectorDialog()
{
}

//------------------------------------------------------------------------------

void SelectorDialog::setSelections(std::vector< std::string > _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void SelectorDialog::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

std::string SelectorDialog::show()
{
    QWidget* parent = qApp->activeWindow();

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString(m_title));

    QListWidget* selectionList = new QListWidget(dialog);
    for( std::string selection :  m_selections)
    {
        selectionList->addItem(QString::fromStdString( selection ));
    }

    QListWidgetItem* firstItem = selectionList->item(0);
    selectionList->setCurrentItem(firstItem);

    QPushButton* okButton     = new QPushButton(QObject::tr("Ok"));
    QPushButton* cancelButton = new QPushButton(QObject::tr("Cancel"));

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    for(auto customButton : m_customButtons)
    {
        hLayout->addWidget(customButton);
        QObject::connect(customButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    }

    QVBoxLayout* vLayout = new QVBoxLayout();
    if(!m_message.empty())
    {
        QLabel* msgText = new QLabel(QString::fromStdString(m_message), dialog);
        vLayout->addWidget( msgText);
    }
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), dialog, SLOT(accept()));

    std::string selection = "";
    if(dialog->exec())
    {
        selection = selectionList->currentItem()->text().toStdString();
    }

    return selection;
}

//------------------------------------------------------------------------------

void SelectorDialog::setMessage(const std::string& msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

void SelectorDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
    QPushButton* button = new QPushButton( QString::fromStdString(label) );
    m_customButtons.push_back( button );
    QObject::connect(button, &QPushButton::clicked, clickedFn);
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
