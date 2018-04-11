/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/dialog/SelectorDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

fwGuiRegisterMacro( ::fwGuiQt::dialog::SelectorDialog, ::fwGui::dialog::ISelectorDialog::REGISTRY_KEY );

namespace fwGuiQt
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
} // namespace fwGuiQt
