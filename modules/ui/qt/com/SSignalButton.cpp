/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "SSignalButton.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QVBoxLayout>

#include <string>

namespace sight::module::ui::qt
{
namespace com
{

static const core::com::Signals::SignalKeyType s_CLICKED_SIG = "clicked";
static const core::com::Signals::SignalKeyType s_TOGGLED_SIG = "toggled";

static const core::com::Slots::SlotKeyType s_SET_CHECKED_SLOT       = "setChecked";
static const core::com::Slots::SlotKeyType s_CHECK_SLOT             = "check";
static const core::com::Slots::SlotKeyType s_UNCHECK_SLOT           = "uncheck";
static const core::com::Slots::SlotKeyType s_SET_IS_EXECUTABLE_SLOT = "setIsExecutable";
static const core::com::Slots::SlotKeyType s_SET_EXECUTABLE_SLOT    = "setExecutable";
static const core::com::Slots::SlotKeyType s_SET_INEXECUTABLE_SLOT  = "setInexecutable";
static const core::com::Slots::SlotKeyType s_SET_VISIBLE_SLOT       = "setVisible";
static const core::com::Slots::SlotKeyType s_SHOW_SLOT              = "show";
static const core::com::Slots::SlotKeyType s_HIDE_SLOT              = "hide";

//-----------------------------------------------------------------------------

SSignalButton::SSignalButton() noexcept
{
    m_sigClicked = newSignal< ClickedSignalType >(s_CLICKED_SIG);
    m_sigToggled = newSignal< ToggledSignalType >(s_TOGGLED_SIG);

    newSlot(s_SET_CHECKED_SLOT, &SSignalButton::setChecked, this);
    newSlot(s_CHECK_SLOT, &SSignalButton::check, this);
    newSlot(s_UNCHECK_SLOT, &SSignalButton::uncheck, this);
    newSlot(s_SET_IS_EXECUTABLE_SLOT, &SSignalButton::setIsExecutable, this);
    newSlot(s_SET_EXECUTABLE_SLOT, &SSignalButton::setExecutable, this);
    newSlot(s_SET_INEXECUTABLE_SLOT, &SSignalButton::setInexecutable, this);
    newSlot(s_SET_VISIBLE_SLOT, &SSignalButton::setVisible, this);
    newSlot(s_SHOW_SLOT, &SSignalButton::show, this);
    newSlot(s_HIDE_SLOT, &SSignalButton::hide, this);
}

//-----------------------------------------------------------------------------

SSignalButton::~SSignalButton() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignalButton::configuring()
{
    this->initialize();

    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");

    if(config)
    {
        core::runtime::ConfigurationElement::sptr checkableCfg = config->findConfigurationElement("checkable");
        if(checkableCfg)
        {
            SIGHT_ASSERT("'checkable' value must be 'true' or 'false'",
                         checkableCfg->getValue() == "true" || checkableCfg->getValue() == "false");
            m_checkable = (checkableCfg->getValue() == "true");
        }

        core::runtime::ConfigurationElement::sptr executableCfg = config->findConfigurationElement("executable");
        if(executableCfg)
        {
            SIGHT_ASSERT("'executable' value must be 'true' or 'false'",
                         executableCfg->getValue() == "true" || executableCfg->getValue() == "false");
            m_executable = (executableCfg->getValue() == "true");
        }

        core::runtime::ConfigurationElement::sptr txtCfg = config->findConfigurationElement("text");
        if(txtCfg)
        {
            m_text = txtCfg->getValue();
        }
        core::runtime::ConfigurationElement::sptr iconCfg = config->findConfigurationElement("icon");
        if(iconCfg)
        {
            m_icon = core::runtime::getModuleResourceFilePath(iconCfg->getValue());
        }

        core::runtime::ConfigurationElement::sptr txt2Cfg = config->findConfigurationElement("text2");
        if(txt2Cfg)
        {
            SIGHT_ASSERT("Button must be 'checkable' in order to defined 'text2'", m_checkable);
            SIGHT_ASSERT("'text' tag must be defined in order to specify 'text2'", !m_text.empty());
            m_text2 = txt2Cfg->getValue();
        }

        core::runtime::ConfigurationElement::sptr icon2Cfg = config->findConfigurationElement("icon2");
        if(icon2Cfg)
        {
            SIGHT_ASSERT("Button must be 'checkable' in order to defined 'icon2'", m_checkable);
            SIGHT_ASSERT("'icon' tag must be defined in order to specify 'icon2'", iconCfg);
            m_icon2 = core::runtime::getModuleResourceFilePath(icon2Cfg->getValue());
        }

        core::runtime::ConfigurationElement::sptr checkedCfg = config->findConfigurationElement("checked");
        if(checkedCfg)
        {
            SIGHT_ASSERT("Button must be 'checkable' in order to defined 'checked'", m_checkable);
            SIGHT_ASSERT("'checked' value must be 'true' or 'false'",
                         checkedCfg->getValue() == "true" || checkedCfg->getValue() == "false");
            m_checkAtStart = (checkedCfg->getValue() == "true");
        }

        core::runtime::ConfigurationElement::sptr widthCfg = config->findConfigurationElement("iconWidth");
        if(widthCfg)
        {
            m_iconWidth = std::stoi(widthCfg->getValue());
        }

        core::runtime::ConfigurationElement::sptr heightCfg = config->findConfigurationElement("iconHeight");
        if(heightCfg)
        {
            m_iconHeight = std::stoi(heightCfg->getValue());
        }

        core::runtime::ConfigurationElement::sptr tooltipCfg = config->findConfigurationElement("toolTip");
        if(tooltipCfg)
        {
            m_toolTip = tooltipCfg->getValue();
        }
    }
}

//-----------------------------------------------------------------------------

void SSignalButton::starting()
{
    this->create();

    auto qtContainer = ::sight::ui::qt::container::QtContainer::dynamicCast( this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    m_button = new QPushButton(QString::fromStdString(m_text));
    m_button->setEnabled(m_executable);
    layout->addWidget(m_button);
    qtContainer->setLayout(layout);

    if(!m_toolTip.empty())
    {
        m_button->setToolTip(QString::fromStdString(m_toolTip));
    }

    if(!m_icon.empty())
    {
        m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
    }
    if(m_iconWidth > 0 && m_iconHeight > 0)
    {
        m_button->setIconSize(QSize(m_iconWidth, m_iconHeight));
    }

    if(m_checkable)
    {
        m_button->setCheckable(true);

        if(m_checkAtStart)
        {
            m_button->setChecked(true);
            if(!m_text2.empty())
            {
                m_button->setText(QString::fromStdString(m_text2));
            }
            if(!m_icon2.empty())
            {
                m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
            }
        }
    }

    QObject::connect(m_button.data(), &QPushButton::clicked, this, &SSignalButton::onClicked );
    QObject::connect(m_button.data(), &QPushButton::toggled, this, &SSignalButton::onToggled );
}

//-----------------------------------------------------------------------------

void SSignalButton::updating()
{
}

//-----------------------------------------------------------------------------

void SSignalButton::stopping()
{
    this->destroy();
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
    if(checked)
    {
        if(!m_text2.empty())
        {
            m_button->setText(QString::fromStdString(m_text2));
        }
        if(!m_icon2.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
        }
    }
    else
    {
        if(!m_text.empty())
        {
            m_button->setText(QString::fromStdString(m_text));
        }
        if(!m_icon.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
        }
    }

    // properly check/uncheck the button when this method is called from the sight slot.
    this->blockSignals(true);
    m_button->setChecked(checked);
    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::check()
{
    this->setChecked(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::uncheck()
{
    this->setChecked(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::setIsExecutable(bool _isExecutable)
{
    m_button->setEnabled(_isExecutable);
}

//-----------------------------------------------------------------------------

void SSignalButton::setExecutable()
{
    m_button->setEnabled(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::setInexecutable()
{
    m_button->setEnabled(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::setVisible(bool _isVisible)
{
    m_button->setVisible(_isVisible);
}

//-----------------------------------------------------------------------------

void SSignalButton::show()
{
    m_button->show();
}

//-----------------------------------------------------------------------------

void SSignalButton::hide()
{
    m_button->hide();
}

//-----------------------------------------------------------------------------

} // namespace com.
} // namespace sight::module::ui::base.
