/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/LoggerDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/foreach.hpp>

#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

fwGuiRegisterMacro( ::fwGuiQml::dialog::LoggerDialog, ::fwGui::dialog::ILoggerDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

LoggerDialog::LoggerDialog(::fwGui::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

LoggerDialog::~LoggerDialog()
{
}

//------------------------------------------------------------------------------

void LoggerDialog::setTitle(const std::string& title)
{
    m_title = title;
}

//------------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_message = message;
}

//------------------------------------------------------------------------------

void LoggerDialog::setLogger(const ::fwLog::Logger::sptr& logger)
{
    m_logger = logger;
}

//------------------------------------------------------------------------------

bool LoggerDialog::show()
{
    QWidget* parent = qApp->activeWindow();

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Create dialog
    m_dialog = new QDialog(parent);
    m_dialog->resize(500, 50);
    m_dialog->setWindowTitle(QString::fromStdString(m_title));
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    m_dialog->setLayout(mainLayout);

    // Disable close button
    m_dialog->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint);

    // Create icon and message widget
    QHBoxLayout* messageLayout = new QHBoxLayout();
    QWidget* messageWidget     = new QWidget();
    messageWidget->setSizePolicy(policy);
    messageWidget->setLayout(messageLayout);
    messageLayout->setAlignment(Qt::AlignTop);

    // Create icon
    QLabel* iconLabel = new QLabel();
    if(m_logger->count(::fwLog::Log::CRITICAL) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else if(m_logger->count(::fwLog::Log::WARNING) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    else
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png");
        iconLabel->setPixmap(QIcon(QString::fromStdString(path.string())).pixmap(48, 48));
    }
    messageLayout->addWidget(iconLabel);

    // Create message
    std::stringstream ss;
    ss << m_message <<
        "<br><br>" << "<b>Log report :</b> " << m_logger->count(::fwLog::Log::CRITICAL) << " critical, " <<
        m_logger->count(::fwLog::Log::WARNING) << " warning and " <<
        m_logger->count(::fwLog::Log::INFORMATION) << " information messages.";

    QLabel* messageLabel = new QLabel(ss.str().c_str());
    messageLayout->addWidget(messageLabel);

    // Create button widget
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QWidget* buttonWidget     = new QWidget();
    buttonWidget->setSizePolicy(policy);
    buttonWidget->setLayout(buttonLayout);

    // Create OK button
    QPushButton* okButton = new QPushButton(tr("Ok"));
    okButton->setSizePolicy(policy);
    buttonLayout->addWidget(okButton);

    // Create CANCEL button
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setSizePolicy(policy);
    buttonLayout->addWidget(cancelButton);

    // Create a checkbox to display the logs
    QCheckBox* checkbox      = new QCheckBox("Details");
    const auto detailshidden = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-hidden.png").string();
    const auto detailsshown  = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-shown.png").string();
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
    ::fwLog::Logger::ConstIteratorType it = m_logger->begin();
    int row = 0;
    for(; it != m_logger->end(); ++it, ++row)
    {
        std::string levelString = "Unknown";
        QIcon levelIcon;
        ::fwLog::Log::LevelType level = it->getLevel();
        if (level == ::fwLog::Log::INFORMATION)
        {
            levelString = "Information";
            const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }
        else if (level == ::fwLog::Log::WARNING)
        {
            levelString = "Warning";
            const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }
        else if (level == ::fwLog::Log::CRITICAL)
        {
            levelString = "Critical";
            const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png");
            levelIcon = QIcon(QString::fromStdString(path.string()));
        }

        QTableWidgetItem* levelItem = new QTableWidgetItem(levelIcon, levelString.c_str());
        levelItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        m_logTableWidget->setItem(row, 0, levelItem);

        QTableWidgetItem* messageItem = new QTableWidgetItem(it->getMessage().c_str());
        messageItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
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
    bool result = m_dialog->exec();

    // Disconnect buttons
    QObject::disconnect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(displayLogs(int)));

    return result;
}

//------------------------------------------------------------------------------

void LoggerDialog::displayLogs(int state)
{
    int width = m_dialog->size().width();

    if(state)
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

} // namespace dialog
} // namespace fwGuiQml
