/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/SSignalButton.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <QVBoxLayout>

#include <string>

namespace guiQt
{
namespace editor
{

static const ::fwCom::Signals::SignalKeyType s_CLICKED_SIG = "clicked";
static const ::fwCom::Signals::SignalKeyType s_TOGGLED_SIG = "toggled";

static const ::fwCom::Slots::SlotKeyType s_SET_CHECKED_SLOT = "setChecked";

fwServicesRegisterMacro( ::gui::editor::IEditor, ::guiQt::editor::SSignalButton, ::fwData::Object );

//-----------------------------------------------------------------------------

SSignalButton::SSignalButton() noexcept :
    m_checkable(false),
    m_checkAtStart(false),
    m_iconWidth(0),
    m_iconHeight(0)
{
    m_sigClicked = newSignal< ClickedSignalType >(s_CLICKED_SIG);
    m_sigToggled = newSignal< ToggledSignalType >(s_TOGGLED_SIG);

    newSlot(s_SET_CHECKED_SLOT, &SSignalButton::setChecked, this);
}

//-----------------------------------------------------------------------------

SSignalButton::~SSignalButton() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignalButton::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    m_button = new QPushButton(QString::fromStdString(m_text));
    layout->addWidget(m_button);
    qtContainer->setLayout(layout);

    if (!m_icon.empty())
    {
        m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
    }
    if (m_iconWidth > 0 && m_iconHeight > 0)
    {
        m_button->setIconSize(QSize(m_iconWidth, m_iconHeight));
    }

    if (m_checkable)
    {
        m_button->setCheckable(true);

        if (m_checkAtStart)
        {
            m_button->setChecked(true);
            if (!m_text2.empty())
            {
                m_button->setText(QString::fromStdString(m_text2));
            }
            if (!m_icon2.empty())
            {
                m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
            }
        }
    }

    QObject::connect(m_button.data(), &QPushButton::clicked, this, &SSignalButton::onClicked );
    QObject::connect(m_button.data(), &QPushButton::toggled, this, &SSignalButton::onToggled );
}

//-----------------------------------------------------------------------------

void SSignalButton::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SSignalButton::configuring()
{
    this->initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("'config' tag is missing", config);

    ::fwRuntime::ConfigurationElement::sptr checkableCfg = config->findConfigurationElement("checkable");
    if (checkableCfg)
    {
        SLM_ASSERT("'checkable' value must be 'true' or 'false'",
                   checkableCfg->getValue() == "true" || checkableCfg->getValue() == "false");
        m_checkable = (checkableCfg->getValue() == "true");
    }

    ::fwRuntime::ConfigurationElement::sptr txtCfg = config->findConfigurationElement("text");
    if (txtCfg)
    {
        m_text = txtCfg->getValue();
    }
    ::fwRuntime::ConfigurationElement::sptr iconCfg = config->findConfigurationElement("icon");
    if (iconCfg)
    {
        m_icon = ::boost::filesystem::path(iconCfg->getValue());
    }

    ::fwRuntime::ConfigurationElement::sptr txt2Cfg = config->findConfigurationElement("text2");
    if (txt2Cfg)
    {
        SLM_ASSERT("Button must be 'checkable' in order to defined 'text2'", m_checkable);
        SLM_ASSERT("'text' tag must be defined in order to specify 'text2'", !m_text.empty());
        m_text2 = txt2Cfg->getValue();
    }

    ::fwRuntime::ConfigurationElement::sptr icon2Cfg = config->findConfigurationElement("icon2");
    if (icon2Cfg)
    {
        SLM_ASSERT("Button must be 'checkable' in order to defined 'icon2'", m_checkable);
        SLM_ASSERT("'icon' tag must be defined in order to specify 'icon2'", iconCfg);
        m_icon2 = ::fwRuntime::getBundleResourceFilePath(icon2Cfg->getValue());
    }

    ::fwRuntime::ConfigurationElement::sptr checkedCfg = config->findConfigurationElement("checked");
    if (checkedCfg)
    {
        SLM_ASSERT("Button must be 'checkable' in order to defined 'checked'", m_checkable);
        SLM_ASSERT("'checked' value must be 'true' or 'false'",
                   checkedCfg->getValue() == "true" || checkedCfg->getValue() == "false");
        m_checkAtStart = (checkedCfg->getValue() == "true");
    }

    ::fwRuntime::ConfigurationElement::sptr widthCfg = config->findConfigurationElement("iconWidth");
    if (widthCfg)
    {
        m_iconWidth = std::stoi(widthCfg->getValue());
    }

    ::fwRuntime::ConfigurationElement::sptr heightCfg = config->findConfigurationElement("iconHeight");
    if (heightCfg)
    {
        m_iconHeight = std::stoi(heightCfg->getValue());
    }
}

//-----------------------------------------------------------------------------

void SSignalButton::onClicked()
{
    m_sigClicked->asyncEmit();
}

//-----------------------------------------------------------------------------

void SSignalButton::onToggled(bool toogled)
{
    this->setChecked(toogled);
    m_sigToggled->asyncEmit(toogled);
}

//-----------------------------------------------------------------------------

void SSignalButton::setChecked(bool checked)
{
    if (checked)
    {
        if (!m_text2.empty())
        {
            m_button->setText(QString::fromStdString(m_text2));
        }
        if (!m_icon2.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
        }
    }
    else
    {
        if (!m_text.empty())
        {
            m_button->setText(QString::fromStdString(m_text));
        }
        if (!m_icon.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
        }
    }
}

//-----------------------------------------------------------------------------

void SSignalButton::updating()
{

}

//-----------------------------------------------------------------------------

void SSignalButton::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui

