/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/dicom/splitter/TagValueInstanceRemoveConfigurableSplitter.hpp"

#include "ui/dicom/widget/QTagSelectorWidget.hpp"

#include <filter/dicom/registry/macros.hpp>

#include <dcmtk/dcmdata/dcdeftag.h>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

fwDicomIOFilterRegisterMacro(sight::ui::dicom::splitter::TagValueInstanceRemoveConfigurableSplitter);

namespace sight::ui::dicom::splitter
{

const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_NAME =
    "Tag value instance remove configurable splitter";
const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_DESCRIPTION =
    "Remove instances if a tag has a certain value.";

//-----------------------------------------------------------------------------

TagValueInstanceRemoveConfigurableSplitter::TagValueInstanceRemoveConfigurableSplitter(
    filter::dicom::IFilter::Key key
) :
    filter::dicom::splitter::TagValueInstanceRemoveSplitter(key)
{
}

//-----------------------------------------------------------------------------

TagValueInstanceRemoveConfigurableSplitter::~TagValueInstanceRemoveConfigurableSplitter()
= default;

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
    auto* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Configure"));
    auto* mainLayout = new QVBoxLayout();
    dialog->setLayout(mainLayout);
    dialog->setMinimumWidth(500);

    // Create tag selectors
    auto* tagSelector =
        new ui::dicom::widget::QTagSelectorWidget();
    tagSelector->setTagValue(m_tag);
    mainLayout->addWidget(tagSelector);

    // Create tag value field
    auto* tagValueWidget = new QWidget();
    mainLayout->addWidget(tagValueWidget);
    auto* tagValueLayout = new QHBoxLayout();
    tagValueWidget->setLayout(tagValueLayout);
    tagValueLayout->addWidget(new QLabel("<b>Value:</b> "));
    auto* tagValuelineEdit = new QLineEdit(m_tagValue.c_str());
    tagValueLayout->addWidget(tagValuelineEdit, 1);
    tagValueLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Create buttons
    auto* buttonBox = new QDialogButtonBox(dialog);
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

} // namespace sight::ui::dicom::splitter
