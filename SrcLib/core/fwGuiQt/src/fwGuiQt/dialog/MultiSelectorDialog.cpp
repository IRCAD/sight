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
#include <QLabel>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/dialog/MultiSelectorDialog.hpp"

REGISTER_BINDING( ::fwGui::dialog::IMultiSelectorDialog, ::fwGuiQt::dialog::MultiSelectorDialog, ::fwGui::dialog::IMultiSelectorDialog::FactoryRegistryKeyType , ::fwGui::dialog::IMultiSelectorDialog::REGISTRY_KEY );

namespace fwGuiQt
{
namespace dialog
{

//------------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog() 
    : m_message(""),
    m_title("")
{}

//------------------------------------------------------------------------------

MultiSelectorDialog::~MultiSelectorDialog()
{}

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
    BOOST_FOREACH( Selections::value_type selection, m_selections)
    {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(selection.first), selectionList);
        item->setCheckState( (selection.second ? Qt::Checked : Qt::Unchecked) );
        selectionList->addItem(item);
    }

    QListWidgetItem* firstItem = selectionList->item(0);
    selectionList->setCurrentItem(firstItem);

    QPushButton* okButton = new QPushButton(tr("Ok"));
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
        int indexItem=0;
        BOOST_FOREACH( Selections::value_type selection, m_selections)
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


