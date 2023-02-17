/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::qt::com
{

static const core::com::Signals::SignalKeyType s_CLICKED_SIG = "clicked";
static const core::com::Signals::SignalKeyType s_TOGGLED_SIG = "toggled";

static const core::com::Slots::SlotKeyType s_SET_CHECKED_SLOT = "setChecked";
static const core::com::Slots::SlotKeyType s_CHECK_SLOT       = "check";
static const core::com::Slots::SlotKeyType s_UNCHECK_SLOT     = "uncheck";
static const core::com::Slots::SlotKeyType s_SET_ENABLED_SLOT = "setEnabled";
static const core::com::Slots::SlotKeyType s_ENABLE_SLOT      = "enable";
static const core::com::Slots::SlotKeyType s_DISABLE_SLOT     = "disable";
static const core::com::Slots::SlotKeyType s_SET_VISIBLE_SLOT = "setVisible";
static const core::com::Slots::SlotKeyType s_SHOW_SLOT        = "show";
static const core::com::Slots::SlotKeyType s_HIDE_SLOT        = "hide";

//-----------------------------------------------------------------------------

SSignalButton::SSignalButton() noexcept :
    m_sigClicked(newSignal<ClickedSignalType>(s_CLICKED_SIG)),
    m_sigToggled(newSignal<ToggledSignalType>(s_TOGGLED_SIG))
{
    newSlot(s_SET_CHECKED_SLOT, &SSignalButton::setChecked, this);
    newSlot(s_CHECK_SLOT, &SSignalButton::check, this);
    newSlot(s_UNCHECK_SLOT, &SSignalButton::uncheck, this);
    newSlot(s_SET_ENABLED_SLOT, &SSignalButton::setEnabled, this);
    newSlot(s_ENABLE_SLOT, &SSignalButton::enable, this);
    newSlot(s_DISABLE_SLOT, &SSignalButton::disable, this);
    newSlot(s_SET_VISIBLE_SLOT, &SSignalButton::setVisible, this);
    newSlot(s_SHOW_SLOT, &SSignalButton::show, this);
    newSlot(s_HIDE_SLOT, &SSignalButton::hide, this);
}

//-----------------------------------------------------------------------------

SSignalButton::~SSignalButton() noexcept =
    default;

//-----------------------------------------------------------------------------

void SSignalButton::configuring()
{
    this->initialize();

    const auto configuration = this->getConfiguration();

    const auto config = configuration.get_child_optional("config");

    if(config.has_value())
    {
        m_checkable    = config->get<bool>("checkable", m_checkable);
        m_checkAtStart = config->get<bool>("checked", m_checkAtStart);
        m_enable       = config->get<bool>("enable", m_enable);

        m_text    = config->get<std::string>("text", m_text);
        m_text2   = config->get<std::string>("text2", m_text2);
        m_toolTip = config->get<std::string>("toolTip", m_toolTip);

        if(const auto icon = config->get_optional<std::string>("icon"); icon.has_value())
        {
            m_icon = core::runtime::getModuleResourceFilePath(icon.value());
        }

        if(const auto icon = config->get_optional<std::string>("icon2"); icon.has_value())
        {
            m_icon2 = core::runtime::getModuleResourceFilePath(icon.value());
        }

        m_iconWidth  = config->get<unsigned int>("iconWidth", m_iconWidth);
        m_iconHeight = config->get<unsigned int>("iconHeight", m_iconHeight);
    }
}

//-----------------------------------------------------------------------------

void SSignalButton::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    auto* layout = new QVBoxLayout();
    m_button = new QPushButton(QString::fromStdString(m_text));
    m_button->setEnabled(m_enable);
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
        m_button->setIconSize(QSize(int(m_iconWidth), int(m_iconHeight)));
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

    QObject::connect(m_button.data(), &QPushButton::clicked, this, &SSignalButton::onClicked);
    QObject::connect(m_button.data(), &QPushButton::toggled, this, &SSignalButton::onToggled);
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

void SSignalButton::onToggled(bool toggled)
{
    this->setChecked(toggled);
    m_sigToggled->asyncEmit(toggled);
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

void SSignalButton::setEnabled(bool _isEnabled)
{
    IEditor::setEnabled(_isEnabled);
    // Keep this in case of SSignalButton is used outside a view container
    m_button->setEnabled(_isEnabled);
}

//-----------------------------------------------------------------------------

void SSignalButton::enable()
{
    this->setEnabled(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::disable()
{
    this->setEnabled(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::setVisible(bool _isVisible)
{
    IEditor::setVisible(_isVisible);
    // Keep this in case of SSignalButton is used outside a view container
    m_button->setVisible(_isVisible);
}

//-----------------------------------------------------------------------------

void SSignalButton::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::com
