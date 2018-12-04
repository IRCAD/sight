/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwDicomIOFilterQt/splitter/TagValueInstanceRemoveConfigurableSplitter.hpp"
#include "fwDicomIOFilterQt/widget/QTagSelectorWidget.hpp"

#include <fwDicomIOFilter/registry/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <dcmtk/dcmdata/dcdeftag.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilterQt::splitter::TagValueInstanceRemoveConfigurableSplitter );

namespace fwDicomIOFilterQt
{
namespace splitter
{

const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_NAME =
    "Tag value instance remove configurable splitter";
const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_DESCRIPTION =
    "Remove instances if a tag has a certain value.";

//-----------------------------------------------------------------------------

TagValueInstanceRemoveConfigurableSplitter::TagValueInstanceRemoveConfigurableSplitter(
    ::fwDicomIOFilter::IFilter::Key key) :
    ::fwDicomIOFilter::splitter::TagValueInstanceRemoveSplitter(key)
{
}

//-----------------------------------------------------------------------------

TagValueInstanceRemoveConfigurableSplitter::~TagValueInstanceRemoveConfigurableSplitter()
{
}

//-----------------------------------------------------------------------------

std::string TagValueInstanceRemoveConfigurableSplitter::getName() const
{
    return TagValueInstanceRemoveConfigurableSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TagValueInstanceRemoveConfigurableSplitter::getDescription() const
{

    return TagValueInstanceRemoveConfigurableSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TagValueInstanceRemoveConfigurableSplitter::isConfigurationRequired() const
{
    return true;
}

//-----------------------------------------------------------------------------

bool TagValueInstanceRemoveConfigurableSplitter::isConfigurableWithGUI() const
{
    return true;
}

//-----------------------------------------------------------------------------

void TagValueInstanceRemoveConfigurableSplitter::configureWithGUI()
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

    // Create tag value field
    QWidget* tagValueWidget = new QWidget();
    mainLayout->addWidget(tagValueWidget);
    QHBoxLayout* tagValueLayout = new QHBoxLayout();
    tagValueWidget->setLayout(tagValueLayout);
    tagValueLayout->addWidget(new QLabel("<b>Value:</b> "));
    QLineEdit* tagValuelineEdit = new QLineEdit(m_tagValue.c_str());
    tagValueLayout->addWidget(tagValuelineEdit, 1);
    tagValueLayout->setContentsMargins(QMargins(0,0,0,0));

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
        m_tag      = tagSelector->getTag();
        m_tagValue = tagValuelineEdit->text().toStdString();
    }
}

} // namespace splitter
} // namespace fwDicomIOFilterQt
