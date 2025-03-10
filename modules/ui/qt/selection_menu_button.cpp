/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "selection_menu_button.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <ui/qt/container/widget.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace sight::module::ui::qt
{

static const core::com::signals::key_t SELECTED_SIG = "selected";

static const core::com::slots::key_t SET_ENABLED_SIG = "set_enabled";
static const core::com::slots::key_t SENABLE_SIG     = "enable";
static const core::com::slots::key_t DISABLE_SIG     = "disable";

selection_menu_button::selection_menu_button() noexcept :
    m_sig_selected(new_signal<selected_signal_t>(SELECTED_SIG))
{
    new_slot(SET_ENABLED_SIG, &selection_menu_button::set_enabled, this);
    new_slot(SENABLE_SIG, &selection_menu_button::enable, this);
    new_slot(DISABLE_SIG, &selection_menu_button::disable, this);
}

//------------------------------------------------------------------------------

void selection_menu_button::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    m_text      = config.get<std::string>("text", m_text);
    m_tool_tip  = config.get<std::string>("toolTip", m_tool_tip);
    m_selection = config.get<int>("selected", m_selection);

    const auto& items = config.get_child("items");
    for(const auto& elem : boost::make_iterator_range(items.equal_range("item")))
    {
        const auto txt  = elem.second.get<std::string>("<xmlattr>.text");
        const int value = elem.second.get<int>("<xmlattr>.value");
        m_items.emplace_back(value, txt);
    }
}

//------------------------------------------------------------------------------

void selection_menu_button::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    m_drop_down_button = new QPushButton(QString::fromStdString(m_text));
    m_drop_down_button->setToolTip(QString::fromStdString(m_tool_tip));
//    m_dropDownButton->setMaximumWidth(40);

    m_p_drop_down_menu = new QMenu();
    m_action_group     = new QActionGroup(m_p_drop_down_menu);

    for(const auto& item : m_items)
    {
        auto* action = new QAction(QString::fromStdString(item.second), m_p_drop_down_menu);
        action->setCheckable(true);
        action->setData(QVariant(item.first));
        m_action_group->addAction(action);
        m_p_drop_down_menu->addAction(action);

        if(item.first == m_selection)
        {
            action->setChecked(true);
        }
    }

    QObject::connect(m_action_group, &QActionGroup::triggered, this, &self_t::on_selection);
    m_drop_down_button->setMenu(m_p_drop_down_menu);

    auto* v_layout = new QVBoxLayout();
    v_layout->addWidget(m_drop_down_button);
    v_layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(v_layout);
}

//------------------------------------------------------------------------------

void selection_menu_button::stopping()
{
    QObject::disconnect(m_action_group, &QActionGroup::triggered, this, &self_t::on_selection);
    for(QAction* action : m_action_group->actions())
    {
        m_action_group->removeAction(action);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void selection_menu_button::updating()
{
}

//------------------------------------------------------------------------------

void selection_menu_button::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void selection_menu_button::on_selection(QAction* _action)
{
    if(_action->isChecked())
    {
        int value = _action->data().toInt();
        m_sig_selected->async_emit(value);
        return;
    }
}

//------------------------------------------------------------------------------

void selection_menu_button::set_enabled(bool _enabled)
{
    m_drop_down_button->setEnabled(_enabled);
}

//------------------------------------------------------------------------------

void selection_menu_button::enable()
{
    this->set_enabled(true);
}

//------------------------------------------------------------------------------

void selection_menu_button::disable()
{
    this->set_enabled(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
