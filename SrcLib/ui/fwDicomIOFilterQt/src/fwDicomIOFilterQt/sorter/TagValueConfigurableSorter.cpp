/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilterQt/sorter/TagValueConfigurableSorter.hpp"
#include "fwDicomIOFilterQt/widget/QTagSelectorWidget.hpp"

#include <fwDicomIOFilter/registry/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <dcmtk/dcmdata/dcdeftag.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilterQt::sorter::TagValueConfigurableSorter );

namespace fwDicomIOFilterQt
{
namespace sorter
{

const std::string TagValueConfigurableSorter::s_FILTER_NAME        = "Tag value configurable sorter";
const std::string TagValueConfigurableSorter::s_FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueConfigurableSorter::TagValueConfigurableSorter(::fwDicomIOFilter::IFilter::Key key) :
    ::fwDicomIOFilter::sorter::TagValueSorter(key)
{
}

//-----------------------------------------------------------------------------

TagValueConfigurableSorter::~TagValueConfigurableSorter()
{
}

//-----------------------------------------------------------------------------

std::string TagValueConfigurableSorter::getName() const
{
    return TagValueConfigurableSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueConfigurableSorter::getDescription() const
{

    return TagValueConfigurableSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueConfigurableSorter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

bool TagValueConfigurableSorter::isConfigurableWithGUI() const
{
    return true;
}

//-----------------------------------------------------------------------------

void TagValueConfigurableSorter::configureWithGUI()
{
    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Configure"));
    QVBoxLayout* mainLayout = new QVBoxLayout();
    dialog->setLayout(mainLayout);
    dialog->setMinimumWidth(500);

    // Create tag selectors
    ::fwDicomIOFilterQt::widget::QTagSelectorWidget* tagSelector =
        new ::fwDicomIOFilterQt::widget::QTagSelectorWidget();
    tagSelector->setTagValue(m_tag);
    mainLayout->addWidget(tagSelector);

    // Create buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(dialog);
    mainLayout->addWidget(buttonBox);
    QPushButton* okButton     = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton* cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    QObject::connect(okButton, SIGNAL(clicked(void)), dialog, SLOT(accept(void)));
    QObject::connect(cancelButton, SIGNAL(clicked(void)), dialog, SLOT(reject(void)));


    int result = dialog->exec();
    if(result == QDialog::Accepted)
    {
        m_tag = tagSelector->getTag();
    }
}

} // namespace sorter
} // namespace fwDicomIOFilterQt
