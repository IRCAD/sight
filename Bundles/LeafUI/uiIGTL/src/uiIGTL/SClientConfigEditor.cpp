/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIGTL/SClientConfigEditor.hpp"

#include <fwData/Object.hpp>
#include <fwServices/Base.hpp>

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

fwServicesRegisterMacro (::gui::editor::IDialogEditor, ::uiIGTL::SClientConfigEditor, ::fwData::Object);


namespace uiIGTL
{
const ::fwCom::Signals::SignalKeyType SClientConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL = "configurationUpdated";

//-----------------------------------------------------------------------------

SClientConfigEditor::SClientConfigEditor()
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
    QVBoxLayout     *mainLayout;
    QHBoxLayout     *hostnameLayout;
    QPushButton     *validateButton;
    QLabel          *portLabel;
    QLabel          *hostnameLabel;
    QPushButton     *cancelButton;
    QHBoxLayout     *portLayout;
    QHBoxLayout     *buttonsLayout;

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
    m_port->setMaximum(::boost::integer_traits< ::boost::uint16_t>::const_max);
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
    m_configDialog.show();
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::onValidate()
{
    ::boost::uint16_t port;
    std::string hostname;

    port     = m_port->value();
    hostname = m_hostname->text().toStdString();
    fwServicesNotifyMacro(this->getLightID(), m_configurationUpdatedSignal, (hostname, port));
    m_configDialog.close();
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::receiving (::fwServices::ObjectMsg::csptr _msg) throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SClientConfigEditor::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace uiIGTL


