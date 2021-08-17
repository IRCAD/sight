/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SSelectionMenuButton.hpp"

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

#include <data/fieldHelper/Image.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace sight::module::ui::qt
{

static const core::com::Signals::SignalKeyType s_SELECTED_SIG = "selected";

static const core::com::Slots::SlotKeyType s_SET_ENABLED_SIG = "setEnabled";
static const core::com::Slots::SlotKeyType s_SENABLE_SIG     = "enable";
static const core::com::Slots::SlotKeyType s_DISABLE_SIG     = "disable";

SSelectionMenuButton::SSelectionMenuButton() noexcept :
    m_text(">"),
    m_selection(0)
{
    m_sigSelected = newSignal<SelectedSignalType>(s_SELECTED_SIG);

    newSlot(s_SET_ENABLED_SIG, &SSelectionMenuButton::setEnabled, this);
    newSlot(s_SENABLE_SIG, &SSelectionMenuButton::enable, this);
    newSlot(s_DISABLE_SIG, &SSelectionMenuButton::disable, this);
}

//------------------------------------------------------------------------------

SSelectionMenuButton::~SSelectionMenuButton() noexcept
{
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::configuring()
{
    this->initialize();

    Configuration txtCfg = m_configuration->findConfigurationElement("text");
    if(txtCfg)
    {
        m_text = txtCfg->getValue();
    }

    Configuration toolTipCfg = m_configuration->findConfigurationElement("toolTip");
    if(toolTipCfg)
    {
        m_toolTip = toolTipCfg->getValue();
    }

    Configuration selectedCfg = m_configuration->findConfigurationElement("selected");
    if(selectedCfg)
    {
        m_selection = std::stoi(selectedCfg->getValue());
    }

    Configuration itemsCfg = m_configuration->findConfigurationElement("items");
    SIGHT_ASSERT("Missing 'items' config", itemsCfg);

    std::vector<Configuration> itemCfgs = itemsCfg->find("item");
    SIGHT_ASSERT("At least two items must be defined", itemCfgs.size() >= 2);
    for(auto itemCfg : itemCfgs)
    {
        SIGHT_ASSERT("Missing 'text' attribute", itemCfg->hasAttribute("text"));
        SIGHT_ASSERT("Missing 'value' attribute", itemCfg->hasAttribute("value"));
        std::string txt = itemCfg->getExistingAttributeValue("text");
        std::string val = itemCfg->getExistingAttributeValue("value");
        int value       = std::stoi(val);
        m_items.push_back(std::make_pair(value, txt));
    }
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::starting()
{
    this->create();

    auto qtContainer = ::sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    m_dropDownButton = new QPushButton(QString::fromStdString(m_text));
    m_dropDownButton->setToolTip(QString::fromStdString(m_toolTip));
//    m_dropDownButton->setMaximumWidth(40);

    m_pDropDownMenu = new QMenu();
    m_actionGroup   = new QActionGroup(m_pDropDownMenu);

    for(auto item : m_items)
    {
        QAction* action = new QAction(QString::fromStdString(item.second), m_pDropDownMenu);
        action->setCheckable(true);
        action->setData(QVariant(item.first));
        m_actionGroup->addAction(action);
        m_pDropDownMenu->addAction(action);

        if(item.first == m_selection)
        {
            action->setChecked(true);
        }
    }

    QObject::connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onSelection(QAction*)));
    m_dropDownButton->setMenu(m_pDropDownMenu);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(m_dropDownButton);
    vLayout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(vLayout);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::stopping()
{
    QObject::connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onSelection(QAction*)));
    for(QAction* action : m_actionGroup->actions())
    {
        m_actionGroup->removeAction(action);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::updating()
{
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::info(std::ostream& _sstream)
{
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::onSelection(QAction* action)
{
    if(action->isChecked())
    {
        int value = action->data().toInt();
        m_sigSelected->asyncEmit(value);
        return;
    }
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::setEnabled(bool enabled)
{
    m_dropDownButton->setEnabled(enabled);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::enable()
{
    this->setEnabled(true);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::disable()
{
    this->setEnabled(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
