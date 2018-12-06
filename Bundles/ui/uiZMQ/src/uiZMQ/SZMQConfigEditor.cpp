/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiZMQ/SZMQConfigEditor.hpp"

#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>

#include <ioZMQ/Patterns.hpp>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include <numeric>

namespace uiZMQ
{
const ::fwCom::Signals::SignalKeyType SZMQConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL = "configurationUpdated";

//-----------------------------------------------------------------------------

SZMQConfigEditor::SZMQConfigEditor()
{
    m_configurationUpdatedSig = ConfigurationUpdatedSignalType::New();

    ::fwCom::HasSignals::m_signals(SZMQConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL,  m_configurationUpdatedSig);
}

//-----------------------------------------------------------------------------

SZMQConfigEditor::~SZMQConfigEditor() noexcept
{
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::configuring()
{
    QVBoxLayout* mainLayout;
    QFormLayout* formLayout;
    QPushButton* validateButton;
    QPushButton* cancelButton;
    QHBoxLayout* buttonLayout;
    QHBoxLayout* portLayout;

    mainLayout     = new QVBoxLayout();
    buttonLayout   = new QHBoxLayout();
    validateButton = new QPushButton("Validate");
    cancelButton   = new QPushButton("Cancel");
    formLayout     = new QFormLayout();
    m_portWidget   = new QWidget();
    portLayout     = new QHBoxLayout();
    m_host         = new QLineEdit();
    m_modeBox      = new QComboBox();
    m_patternBox   = new QComboBox();
    m_title        = new QLabel("Host");
    m_protocolBox  = new QComboBox();
    m_port         = new QSpinBox();

    m_modeBox->addItem(QString::fromStdString("Client"), QVariant(static_cast<int>(::zmqNetwork::Socket::Client)));
    m_modeBox->addItem(QString::fromStdString("Server"), QVariant(static_cast<int>(::zmqNetwork::Socket::Server)));
    portLayout->addWidget(new QLabel("Port : "));
    portLayout->addWidget(m_port);
    m_protocolBox->addItem(QString::fromStdString("tcp"));
    m_protocolBox->addItem(QString::fromStdString("ipc"));
    m_modeBox->setCurrentIndex(0);
    this->initPatternBox();

    m_port->setMinimum(0);
    m_port->setMaximum(std::numeric_limits<std::uint16_t>::max());

    formLayout->addRow(QString::fromStdString("Protocol : "), m_protocolBox);
    formLayout->addRow(QString::fromStdString("Mode : "), m_modeBox);
    formLayout->addRow(QString::fromStdString("Pattern : "), m_patternBox);
    formLayout->addRow(m_title, m_host);
    formLayout->addRow(m_portWidget);

    buttonLayout->addWidget(validateButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    m_dialog.setModal(true);
    m_dialog.setLayout(mainLayout);
    m_portWidget->setLayout(portLayout);

    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidate()));
    QObject::connect(cancelButton, SIGNAL(clicked()), &m_dialog, SLOT(close()));
    m_protocol = m_protocolBox->currentText().toStdString() + "://";
    QObject::connect(m_protocolBox, SIGNAL(currentIndexChanged(const QString&)), this,
                     SLOT(onChangeProtocol(QString const&)));
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::starting()
{
    FW_DEPRECATED_MSG("'uiZMQ' bundle and the associated services are no longer supported.", "19.0");
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::stopping()
{
    m_dialog.close();
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::updating()
{
    m_dialog.show();
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::onChangeProtocol(QString const& protocol)
{
    m_protocol = protocol.toStdString() + "://";
    if (m_protocol == "ipc://")
    {
        m_portWidget->hide();
        m_title->setText("File : ");
    }
    else
    {
        m_portWidget->show();
        m_title->setText("Host : ");
    }
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::onValidate()
{
    std::stringstream stream;
    std::string host;
    ::zmqNetwork::Socket::PatternMode pattern;
    ::zmqNetwork::Socket::SocketMode sockMode;

    sockMode = static_cast< ::zmqNetwork::Socket::SocketMode>(m_modeBox->currentData().toInt());
    pattern  = static_cast< ::zmqNetwork::Socket::PatternMode>(m_patternBox->currentData().toInt());
    if (m_protocol == "tcp://")
    {
        stream << m_protocol << m_host->text().toStdString()  << ":" << m_port->value();
    }
    else
    {
        stream << m_protocol << m_host->text().toStdString();
    }
    stream >> host;
    m_configurationUpdatedSig->asyncEmit(pattern, sockMode, host);
    m_dialog.close();
}

//-----------------------------------------------------------------------------

void SZMQConfigEditor::swapping()
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace uiZMQ
