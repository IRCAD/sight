/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/dicom/sorter/TagValueConfigurableSorter.hpp"

#include "ui/dicom/widget/QTagSelectorWidget.hpp"

#include <filter/dicom/registry/macros.hpp>

#include <dcmtk/dcmdata/dcdeftag.h>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

fwDicomIOFilterRegisterMacro(::sight::ui::dicom::sorter::TagValueConfigurableSorter);

namespace sight::ui::dicom
{

namespace sorter
{

const std::string TagValueConfigurableSorter::s_FILTER_NAME        = "Tag value configurable sorter";
const std::string TagValueConfigurableSorter::s_FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

TagValueConfigurableSorter::TagValueConfigurableSorter(filter::dicom::IFilter::Key key) :
    filter::dicom::sorter::TagValueSorter(key)
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
    ui::dicom::widget::QTagSelectorWidget* tagSelector =
        new ui::dicom::widget::QTagSelectorWidget();
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

} // namespace sight::ui::dicom
