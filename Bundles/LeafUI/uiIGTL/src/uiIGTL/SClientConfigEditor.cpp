/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIGTL/SClientConfigEditor.hpp"

#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>

fwServicesRegisterMacro (::gui::editor::IDialogEditor, ::uiIGTL::SClientConfigEditor, ::fwData::Object);


namespace uiIGTL
{
const ::fwCom::Signals::SignalKeyType SClientConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL = "configurationUpdated";

//-----------------------------------------------------------------------------

SClientConfigEditor::SClientConfigEditor() : m_defaultHostName("127.0.0.1"),m_defaultPort(0)
{
    m_configurationUpdatedSignal = ConfigurationUpdatedSignalType::New();

    ::fwCom::HasSignals::m_signals (SClientConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL,  m_configurationUpdatedSignal);
}

//-----------------------------------------------------------------------------

SClientConfigEditor::~SClientConfigEditor() throw()
{
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::configuring() throw (::fwTools::Failed)
{
    QVBoxLayout* mainLayout;
    QHBoxLayout* hostnameLayout;
    QPushButton* validateButton;
    QLabel* portLabel;
    QLabel* hostnameLabel;
    QPushButton* cancelButton;
    QHBoxLayout* portLayout;
    QHBoxLayout* buttonsLayout;

    mainLayout     = new QVBoxLayout();
    validateButton = new QPushButton("Validate");
    cancelButton   = new QPushButton("Cancel");
    portLayout     = new QHBoxLayout();
    hostnameLayout = new QHBoxLayout();
    portLabel      = new QLabel("Enter port :");
    buttonsLayout  = new QHBoxLayout();
    hostnameLabel  = new QLabel("Enter hostname :");
    m_port         = new QSpinBox();
    m_hostname     = new QLineEdit();

    mainLayout->addLayout(hostnameLayout);
    mainLayout->addLayout(portLayout);
    mainLayout->addLayout(buttonsLayout);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(m_port);
    hostnameLayout->addWidget(hostnameLabel);
    hostnameLayout->addWidget(m_hostname);

    buttonsLayout->addWidget(validateButton);
    buttonsLayout->addWidget(cancelButton);
    m_port->setMinimum(0);
    m_port->setMaximum(::boost::integer_traits< std::uint16_t>::const_max);
    m_configDialog.setModal(true);
    m_configDialog.setLayout(mainLayout);

    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidate()));
    QObject::connect(cancelButton, SIGNAL(clicked()), &m_configDialog, SLOT(close()));
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::starting() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::stopping() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::updating() throw (::fwTools::Failed)
{
    m_hostname->setText(QString::fromStdString(m_defaultHostName));
    m_port->setValue(m_defaultPort);
    m_configDialog.show();
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::onValidate()
{
    std::uint16_t port;
    std::string hostname;

    port              = m_port->value();
    hostname          = m_hostname->text().toStdString();
    m_defaultHostName = hostname;
    m_defaultPort     = port;
    m_configurationUpdatedSignal->asyncEmit (hostname, port);
    m_configDialog.close();
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace uiIGTL


