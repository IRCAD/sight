/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/qt/dialog/logger.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>

#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::ui::qt::dialog
{

//------------------------------------------------------------------------------

void logger::set_title(const std::string& _title)
{
    m_title = _title;
}

//------------------------------------------------------------------------------

void logger::set_message(const std::string& _message)
{
    m_message = _message;
}

//------------------------------------------------------------------------------

void logger::set_logger(const core::log::logger::sptr& _logger)
{
    m_logger = _logger;
}

//------------------------------------------------------------------------------

bool logger::show()
{
    QWidget* parent = qApp->activeWindow();

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Create dialog
    m_dialog = new QDialog(parent);
    m_dialog->resize(500, 50);
    m_dialog->setWindowTitle(QString::fromStdString(m_title));
    auto* main_layout = new QVBoxLayout();
    main_layout->setAlignment(Qt::AlignTop);
    m_dialog->setLayout(main_layout);

    // Disable close button
    m_dialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // Create icon and message widget
    auto* message_layout = new QHBoxLayout();
    auto* message_widget = new QWidget();
    message_widget->setSizePolicy(policy);
    message_widget->setLayout(message_layout);
    message_layout->setAlignment(Qt::AlignTop);

    // Create icon
    auto* icon_label = new QLabel();
    if(m_logger->count(core::log::log::critical) > 0)
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/critical.png");
        icon_label->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else if(m_logger->count(core::log::log::warning) > 0)
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/warning.png");
        icon_label->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/information.png");
        icon_label->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }

    message_layout->addWidget(icon_label);

    // Create message
    std::stringstream ss;
    ss << m_message
    << "<br><br>" << "<b>Log report :</b> " << m_logger->count(core::log::log::critical) << " critical, "
    << m_logger->count(core::log::log::warning) << " warning and "
    << m_logger->count(core::log::log::information) << " information messages.";

    auto* message_label = new QLabel(ss.str().c_str());
    message_layout->addWidget(message_label);

    // Create button widget
    auto* button_layout = new QHBoxLayout();
    auto* button_widget = new QWidget();
    button_widget->setSizePolicy(policy);
    button_widget->setLayout(button_layout);

    // Create OK button
    auto* ok_button = new QPushButton(tr("Ok"));
    ok_button->setObjectName(ok_button->text());
    ok_button->setSizePolicy(policy);
    button_layout->addWidget(ok_button);

    // Create CANCEL button
    auto* cancel_button = new QPushButton(tr("Cancel"));
    cancel_button->setObjectName(cancel_button->text());
    cancel_button->setSizePolicy(policy);
    button_layout->addWidget(cancel_button);

    // Create a checkbox to display the logs
    auto* checkbox = new QCheckBox("Details");
    checkbox->setObjectName(checkbox->text());
    const auto detailshidden =
        core::runtime::get_library_resource_file_path("sight::ui::qt/details-hidden.png").string();
    const auto detailsshown =
        core::runtime::get_library_resource_file_path("sight::ui::qt/details-shown.png").string();
    std::string style_sheet;
    style_sheet += "QCheckBox::indicator:unchecked { image: url(" + detailshidden + "); }";
    style_sheet += "QCheckBox::indicator:checked { image: url(" + detailsshown + "); }";
    checkbox->setStyleSheet(QString::fromStdString(style_sheet));

    // Create log table
    m_log_table_widget = new QTableWidget(static_cast<int>(m_logger->count()), 2);
    m_log_table_widget->setHorizontalHeaderItem(0, new QTableWidgetItem("Level"));
    m_log_table_widget->setHorizontalHeaderItem(1, new QTableWidgetItem("Message"));
    m_log_table_widget->setColumnWidth(0, 120);
    m_log_table_widget->horizontalHeader()->setStretchLastSection(true);

    // Fill log table
    auto it = m_logger->begin();
    int row = 0;
    for( ; it != m_logger->end() ; ++it, ++row)
    {
        std::string level_string = "Unknown";
        QIcon level_icon;
        core::log::log::level_t level = it->get_level();
        if(level == core::log::log::information)
        {
            level_string = "Information";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/information.png");
            level_icon = QIcon(QString::fromStdString(path.string()));
        }
        else if(level == core::log::log::warning)
        {
            level_string = "Warning";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/warning.png");
            level_icon = QIcon(QString::fromStdString(path.string()));
        }
        else if(level == core::log::log::critical)
        {
            level_string = "Critical";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/critical.png");
            level_icon = QIcon(QString::fromStdString(path.string()));
        }

        auto* level_item = new QTableWidgetItem(level_icon, level_string.c_str());
        level_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_log_table_widget->setItem(row, 0, level_item);

        auto* message_item = new QTableWidgetItem(it->get_message().c_str());
        message_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_log_table_widget->setItem(row, 1, message_item);
    }

    // Add widget to dialog
    main_layout->addWidget(message_widget, 0, Qt::AlignLeft);
    main_layout->addWidget(checkbox);
    main_layout->addWidget(m_log_table_widget);
    main_layout->addWidget(button_widget, 0, Qt::AlignRight);

    // Hide log table
    m_log_table_widget->hide();

    // Connect buttons
    QObject::connect(ok_button, SIGNAL(clicked()), m_dialog, SLOT(accept()));
    QObject::connect(cancel_button, SIGNAL(clicked()), m_dialog, SLOT(reject()));
    QObject::connect(checkbox, &QCheckBox::stateChanged, this, &logger::display_logs);

    // Show dialog and return result
    bool result = m_dialog->exec() != 0;

    // Disconnect buttons
    QObject::disconnect(checkbox, &QCheckBox::stateChanged, this, &logger::display_logs);

    return result;
}

//------------------------------------------------------------------------------

void logger::display_logs(int _state)
{
    int width = m_dialog->size().width();

    if(_state != 0)
    {
        m_log_table_widget->show();
    }
    else
    {
        m_log_table_widget->hide();
    }

    m_dialog->adjustSize();
    m_dialog->resize(width, m_dialog->baseSize().height());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
