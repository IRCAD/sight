/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "ex_collapsible_section_editor.hpp"

#include <ui/qt/container/widget.hpp>
#include <ui/qt/widget/collapsible_section.hpp>

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

namespace ex_collapsible_section
{

//------------------------------------------------------------------------------

void ex_collapsible_section_editor::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(base_id());
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    auto* layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    // Create a collapsible section widget.
    auto* section = new sight::ui::qt::widget::collapsible_section(
        "Section",
        200,
        layout->widget()
    );

    // Create the container to add inside the collapsible section.
    auto* box_section = new QVBoxLayout();

    // Add 2 QWidget in the collapsible section.
    box_section->addWidget(new QLabel("Test section", section));
    box_section->addWidget(new QCheckBox("Button in Section", section));

    // Button to add dynamically a new label inside collapsible section.
    auto* add_label_button = new QPushButton("Add new label");
    add_label_button->setObjectName("add_label_button");
    QObject::connect(
        add_label_button,
        &QPushButton::clicked,
        [section, box_section]()
        {
            auto* label_to_add = new QLabel("New label added", section);
            label_to_add->setObjectName(QString::fromStdString("name/QLabel"));
            box_section->addWidget(label_to_add);
            section->update_heights(true);
        });

    // Button to delete dynamically a label inside collapsible section.
    auto* remove_label_button = new QPushButton("Remove label");
    remove_label_button->setObjectName("remove_label_button");
    QObject::connect(
        remove_label_button,
        &QPushButton::clicked,
        [section, box_section, qt_container]()
        {
            auto* label_to_remove =
                qt_container->get_qt_container()->findChild<QLabel*>(QString::fromStdString("name/QLabel"));
            box_section->removeWidget(label_to_remove);
            delete label_to_remove;
            section->update_heights(true);
        });

    section->set_content_layout(box_section);

    auto* under_label = new QLabel("An under section label");
    under_label->setObjectName("under_section_label");

    layout->addWidget(section);
    layout->addWidget(under_label);
    layout->addWidget(add_label_button);
    layout->addWidget(remove_label_button);

    qt_container->set_layout(layout);
}

//------------------------------------------------------------------------------

void ex_collapsible_section_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void ex_collapsible_section_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void ex_collapsible_section_editor::updating()
{
}

} // namespace ex_collapsible_section.
