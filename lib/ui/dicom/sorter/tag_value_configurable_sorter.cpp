/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/dicom/sorter/tag_value_configurable_sorter.hpp"

#include "ui/dicom/widget/tag_selector_widget.hpp"

#include <filter/dicom/registry/macros.hpp>

#include <dcmtk/dcmdata/dcdeftag.h>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

SIGHT_REGISTER_DICOM_FILTER(sight::ui::dicom::sorter::tag_value_configurable_sorter);

namespace sight::ui::dicom::sorter
{

const std::string tag_value_configurable_sorter::FILTER_NAME        = "Tag value configurable sorter";
const std::string tag_value_configurable_sorter::FILTER_DESCRIPTION =
    "Sort instances using a tag value.";

//-----------------------------------------------------------------------------

std::string tag_value_configurable_sorter::get_name() const
{
    return tag_value_configurable_sorter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string tag_value_configurable_sorter::get_description() const
{
    return tag_value_configurable_sorter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool tag_value_configurable_sorter::is_configuration_required() const
{
    return true;
}

//-----------------------------------------------------------------------------

bool tag_value_configurable_sorter::is_configurable_with_gui() const
{
    return true;
}

//-----------------------------------------------------------------------------

void tag_value_configurable_sorter::configure_with_gui()
{
    auto* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Configure"));
    auto* main_layout = new QVBoxLayout();
    dialog->setLayout(main_layout);
    dialog->setMinimumWidth(500);

    // Create tag selectors
    auto* tag_selector =
        new ui::dicom::widget::tag_selector_widget();
    tag_selector->set_tag_value(m_tag);
    main_layout->addWidget(tag_selector);

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
        m_tag = tag_selector->get_tag();
    }
}

} // namespace sight::ui::dicom::sorter
