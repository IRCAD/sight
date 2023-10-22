/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

SIGHT_REGISTER_DICOM_FILTER(sight::ui::dicom::splitter::TagValueInstanceRemoveConfigurableSplitter);

namespace sight::ui::dicom::splitter
{

const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_NAME =
    "Tag value instance remove configurable splitter";
const std::string TagValueInstanceRemoveConfigurableSplitter::s_FILTER_DESCRIPTION =
    "Remove instances if a tag has a certain value.";

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
    auto* main_layout = new QVBoxLayout();
    dialog->setLayout(main_layout);
    dialog->setMinimumWidth(500);

    // Create tag selectors
    auto* tag_selector =
        new ui::dicom::widget::QTagSelectorWidget();
    tag_selector->setTagValue(m_tag);
    main_layout->addWidget(tag_selector);

    // Create tag value field
    auto* tag_value_widget = new QWidget();
    main_layout->addWidget(tag_value_widget);
    auto* tag_value_layout = new QHBoxLayout();
    tag_value_widget->setLayout(tag_value_layout);
    tag_value_layout->addWidget(new QLabel("<b>Value:</b> "));
    auto* tag_valueline_edit = new QLineEdit(m_tagValue.c_str());
    tag_value_layout->addWidget(tag_valueline_edit, 1);
    tag_value_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Create buttons
    auto* button_box = new QDialogButtonBox(dialog);
    main_layout->addWidget(button_box);
    QPushButton* ok_button     = button_box->addButton(QDialogButtonBox::Ok);
    QPushButton* cancel_button = button_box->addButton(QDialogButtonBox::Cancel);

    QObject::connect(ok_button, SIGNAL(clicked(void)), dialog, SLOT(accept(void)));
    QObject::connect(cancel_button, SIGNAL(clicked(void)), dialog, SLOT(reject(void)));

    int result = dialog->exec();
    if(result == QDialog::Accepted)
    {
        m_tag      = tag_selector->getTag();
        m_tagValue = tag_valueline_edit->text().toStdString();
    }
}

} // namespace sight::ui::dicom::splitter
