/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilterQt/splitter/TagValueConfigurableSplitter.hpp"
#include "fwDicomIOFilterQt/widget/QTagSelectorWidget.hpp"

#include <fwDicomIOFilter/registry/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <dcmtk/dcmdata/dcdeftag.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilterQt::splitter::TagValueConfigurableSplitter );

namespace fwDicomIOFilterQt
{
namespace splitter
{

const std::string TagValueConfigurableSplitter::s_FILTER_NAME        = "Tag value configurable splitter";
const std::string TagValueConfigurableSplitter::s_FILTER_DESCRIPTION =
    "Split instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueConfigurableSplitter::TagValueConfigurableSplitter(::fwDicomIOFilter::IFilter::Key key) :
    ::fwDicomIOFilter::splitter::TagValueSplitter(key)
{
}

//-----------------------------------------------------------------------------

TagValueConfigurableSplitter::~TagValueConfigurableSplitter()
{
}

//-----------------------------------------------------------------------------

std::string TagValueConfigurableSplitter::getName() const
{
    return TagValueConfigurableSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueConfigurableSplitter::getDescription() const
{

    return TagValueConfigurableSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueConfigurableSplitter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

bool TagValueConfigurableSplitter::isConfigurableWithGUI() const
{
    return true;
}

//-----------------------------------------------------------------------------

void TagValueConfigurableSplitter::configureWithGUI()
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

} // namespace splitter
} // namespace fwDicomIOFilterQt
