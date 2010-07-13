/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <QApplication>
#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/Selector.hpp"

REGISTER_BINDING( ::fwGui::ISelector, ::fwGuiQt::Selector, ::fwGui::ISelector::FactoryRegistryKeyType , ::fwGui::ISelector::REGISTRY_KEY );

namespace fwGuiQt
{

//------------------------------------------------------------------------------

Selector::Selector() : m_title("")
{}

//------------------------------------------------------------------------------

Selector::~Selector()
{}

//------------------------------------------------------------------------------

void Selector::setSelections(std::vector< std::string > _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void Selector::setTitle(std::string _title)
{
    this->m_title = _title;
}

//------------------------------------------------------------------------------

std::string Selector::show()
{
    QWidget *parent = qApp->activeWindow();

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString(m_title));

    QListWidget *selectionList = new QListWidget(dialog);
    BOOST_FOREACH( std::string selection, m_selections)
    {
        selectionList->addItem(QString::fromStdString ( selection ));
    }

    //    selectionList->setCurrentIndex(0);
    QPushButton* okButton = new QPushButton("Ok");
    QPushButton* cancelButton = new QPushButton("Cancel");

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    std::string selection = "";
    if(dialog->exec())
    {
        selection = selectionList->currentItem()->text().toStdString();
    }

    return selection;
}

//------------------------------------------------------------------------------

} // namespace fwGuiQt


