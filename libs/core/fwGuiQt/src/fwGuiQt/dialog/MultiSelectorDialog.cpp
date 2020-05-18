/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwGuiQt/dialog/MultiSelectorDialog.hpp"

#include <fwCore/base.hpp>
#include <fwGui/registry/macros.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QPushButton>
#include <QVBoxLayout>

fwGuiRegisterMacro( ::fwGuiQt::dialog::MultiSelectorDialog, ::fwGui::dialog::IMultiSelectorDialog::REGISTRY_KEY );

namespace fwGuiQt
{
namespace dialog
{

//------------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog(::fwGui::GuiBaseObject::Key key)
    : m_message(""),
      m_title("")
{
}

//------------------------------------------------------------------------------

MultiSelectorDialog::~MultiSelectorDialog()
{
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    QWidget *parent = qApp->activeWindow();

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString(m_title));

    QListWidget *selectionList = new QListWidget(dialog);
    for( Selections::value_type selection :  m_selections)
    {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(selection.first), selectionList);
        item->setCheckState( (selection.second ? Qt::Checked : Qt::Unchecked) );
        selectionList->addItem(item);
    }

    QListWidgetItem* firstItem = selectionList->item(0);
    selectionList->setCurrentItem(firstItem);

    QPushButton* okButton     = new QPushButton(tr("Ok"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout *vLayout = new QVBoxLayout();
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
    QObject::connect(selectionList, SIGNAL(itemDoubleClicked( QListWidgetItem * )), dialog, SLOT(accept()));

    Selections selections;
    if(dialog->exec())
    {
        int indexItem = 0;
        for( Selections::value_type selection :  m_selections)
        {
            selections[selection.first] = (selectionList->item(indexItem)->checkState() == Qt::Checked);
            indexItem++;
        }
    }
    return selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string &msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQt


