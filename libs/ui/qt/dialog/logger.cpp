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

void logger::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

void logger::setMessage(const std::string& message)
{
    m_message = message;
}

//------------------------------------------------------------------------------

void logger::setLogger(const core::log::logger::sptr& logger)
{
    m_logger = logger;
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
    auto* mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    m_dialog->setLayout(mainLayout);

    // Disable close button
    m_dialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // Create icon and message widget
    auto* messageLayout = new QHBoxLayout();
    auto* messageWidget = new QWidget();
    messageWidget->setSizePolicy(policy);
    messageWidget->setLayout(messageLayout);
    messageLayout->setAlignment(Qt::AlignTop);

    // Create icon
    auto* iconLabel = new QLabel();
    if(m_logger->count(core::log::log::CRITICAL) > 0)
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/critical.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else if(m_logger->count(core::log::log::WARNING) > 0)
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/warning.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else
    {
        const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/information.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }

    messageLayout->addWidget(iconLabel);

    // Create message
    std::stringstream ss;
    ss << m_message
    << "<br><br>" << "<b>Log report :</b> " << m_logger->count(core::log::log::CRITICAL) << " critical, "
    << m_logger->count(core::log::log::WARNING) << " warning and "
    << m_logger->count(core::log::log::INFORMATION) << " information messages.";

    auto* messageLabel = new QLabel(ss.str().c_str());
    messageLayout->addWidget(messageLabel);

    // Create button widget
    auto* buttonLayout = new QHBoxLayout();
    auto* buttonWidget = new QWidget();
    buttonWidget->setSizePolicy(policy);
    buttonWidget->setLayout(buttonLayout);

    // Create OK button
    auto* okButton = new QPushButton(tr("Ok"));
    okButton->setObjectName(okButton->text());
    okButton->setSizePolicy(policy);
    buttonLayout->addWidget(okButton);

    // Create CANCEL button
    auto* cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setObjectName(cancelButton->text());
    cancelButton->setSizePolicy(policy);
    buttonLayout->addWidget(cancelButton);

    // Create a checkbox to display the logs
    auto* checkbox = new QCheckBox("Details");
    checkbox->setObjectName(checkbox->text());
    const auto detailshidden =
        core::runtime::get_library_resource_file_path("sight::ui::qt/details-hidden.png").string();
    const auto detailsshown =
        core::runtime::get_library_resource_file_path("sight::ui::qt/details-shown.png").string();
    std::string styleSheet;
    styleSheet += "QCheckBox::indicator:unchecked { image: url(" + detailshidden + "); }";
    styleSheet += "QCheckBox::indicator:checked { image: url(" + detailsshown + "); }";
    checkbox->setStyleSheet(QString::fromStdString(styleSheet));

    // Create log table
    m_logTableWidget = new QTableWidget(static_cast<int>(m_logger->count()), 2);
    m_logTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Level"));
    m_logTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Message"));
    m_logTableWidget->setColumnWidth(0, 120);
    m_logTableWidget->horizontalHeader()->setStretchLastSection(true);

    // Fill log table
    auto it = m_logger->begin();
    int row = 0;
    for( ; it != m_logger->end() ; ++it, ++row)
    {
        std::string levelString = "Unknown";
        QIcon levelIcon;
        core::log::log::level_type level = it->get_level();
        if(level == core::log::log::INFORMATION)
        {
            levelString = "Information";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/information.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }
        else if(level == core::log::log::WARNING)
        {
            levelString = "Warning";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/warning.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }
        else if(level == core::log::log::CRITICAL)
        {
            levelString = "Critical";
            const auto path = core::runtime::get_library_resource_file_path("sight::ui::qt/critical.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }

        auto* levelItem = new QTableWidgetItem(levelIcon, levelString.c_str());
        levelItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_logTableWidget->setItem(row, 0, levelItem);

        auto* messageItem = new QTableWidgetItem(it->get_message().c_str());
        messageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_logTableWidget->setItem(row, 1, messageItem);
    }

    // Add widget to dialog
    mainLayout->addWidget(messageWidget, 0, Qt::AlignLeft);
    mainLayout->addWidget(checkbox);
    mainLayout->addWidget(m_logTableWidget);
    mainLayout->addWidget(buttonWidget, 0, Qt::AlignRight);

    // Hide log table
    m_logTableWidget->hide();

    // Connect buttons
    QObject::connect(okButton, SIGNAL(clicked()), m_dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), m_dialog, SLOT(reject()));
    QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(displayLogs(int)));

    // Show dialog and return result
    bool result = m_dialog->exec() != 0;

    // Disconnect buttons
    QObject::disconnect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(displayLogs(int)));

    return result;
}

//------------------------------------------------------------------------------

void logger::displayLogs(int state)
{
    int width = m_dialog->size().width();

    if(state != 0)
    {
        m_logTableWidget->show();
    }
    else
    {
        m_logTableWidget->hide();
    }

    m_dialog->adjustSize();
    m_dialog->resize(width, m_dialog->baseSize().height());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt::dialog
