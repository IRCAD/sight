/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "icon_speed_dial.hpp"

#include "core/thread/worker.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <service/op.hpp>

#include <ui/__/action.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QBoxLayout>
#include <QResizeEvent>

class ResizeActionsFilter : public QObject
{
private:

    std::vector<QWidget*> m_actions;

public:

    explicit ResizeActionsFilter(std::vector<QWidget*> _actions) :
        m_actions(std::move(_actions))
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*target*/, QEvent* _e) override
    {
        if(_e->type() == QEvent::Resize)
        {
            auto* resize_event = static_cast<QResizeEvent*>(_e);
            std::ranges::for_each(m_actions, [resize_event](QWidget* _w){_w->setFixedSize(resize_event->size());});
        }

        return false;
    }
};

class ResizeIconFilter : public QObject
{
public:

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _target, QEvent* _e) override
    {
        if(_e->type() == QEvent::Resize)
        {
            auto* push_button = qobject_cast<QPushButton*>(_target);
            push_button->setIconSize(push_button->size());
        }

        return false;
    }
};

namespace sight::module::ui::qt
{

icon_speed_dial::icon_speed_dial()
{
    new_slot(icon_speed_dial::slots::FOLD, [this]{m_speedDial->fold();});
    new_slot(icon_speed_dial::slots::UNFOLD, [this]{m_speedDial->unfold();});
    new_slot(icon_speed_dial::slots::UPDATE_ACTIONS, &icon_speed_dial::updateActions, this);
}

//------------------------------------------------------------------------------

void icon_speed_dial::configuring()
{
    initialize();

    const auto& config = this->get_config();

    auto direction = config.get<std::string>("config.<xmlattr>.direction");
    if(direction == "up")
    {
        m_direction = sight::ui::qt::widget::speed_dial::Direction::UP;
    }
    else if(direction == "right")
    {
        m_direction = sight::ui::qt::widget::speed_dial::Direction::RIGHT;
    }
    else if(direction == "down")
    {
        m_direction = sight::ui::qt::widget::speed_dial::Direction::DOWN;
    }
    else if(direction == "left")
    {
        m_direction = sight::ui::qt::widget::speed_dial::Direction::LEFT;
    }
    else
    {
        SIGHT_ASSERT("Invalid direction " << direction << " for speed_dial " << get_id(), false);
    }

    m_spacing           = config.get("config.<xmlattr>.spacing", -1);
    m_icon              = config.get<std::string>("config.<xmlattr>.icon");
    m_unfoldedIcon      = config.get("config.<xmlattr>.unfoldedIcon", "");
    m_animationDuration = config.get("config.<xmlattr>.animationDuration", -1);
    std::ranges::transform(
        boost::make_iterator_range(config.get_child("actions").equal_range("action")),
        std::back_inserter(m_actions),
        [](const auto& _action) -> action
        {
            return {
                .sid      = _action.second.template get<std::string>("<xmlattr>.sid"),
                .name     = _action.second.get("<xmlattr>.name", ""),
                .icon     = _action.second.template get<std::string>("<xmlattr>.icon"),
                .shortcut = _action.second.get("<xmlattr>.shortcut", "")
            };
        });
}

//------------------------------------------------------------------------------

void icon_speed_dial::starting()
{
    create();

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    auto* layout      = new QBoxLayout(QBoxLayout::TopToBottom);
    m_speedDial = new sight::ui::qt::widget::speed_dial(m_direction);
    m_speedDial->setObjectName(QString::fromStdString(service_id));
    if(m_spacing >= 0)
    {
        m_speedDial->setSpacing(m_spacing);
    }

    if(!m_icon.empty())
    {
        QIcon icon = m_speedDial->icon();
        icon.addFile(QString::fromStdString(core::runtime::get_module_resource_file_path(m_icon).string()));
        m_speedDial->setIcon(icon);
    }

    if(!m_unfoldedIcon.empty())
    {
        QIcon icon = m_speedDial->icon();
        icon.addFile(
            QString::fromStdString(core::runtime::get_module_resource_file_path(m_unfoldedIcon).string()),
            {},
            QIcon::Normal,
            QIcon::On
        );
        m_speedDial->setIcon(icon);
    }

    if(m_animationDuration >= 0)
    {
        m_speedDial->setAnimationDuration(m_animationDuration);
    }

    for(const action& action : m_actions)
    {
        auto* q_action = new QPushButton;
        q_action->setFixedSize(m_speedDial->size());
        q_action->setObjectName(QString::fromStdString(action.name.empty() ? action.sid : action.name));
        std::string sid = action.sid;
        auto service    = std::dynamic_pointer_cast<sight::ui::action>(sight::service::get(sid));
        SIGHT_ASSERT("icon_speed_dial only supports action", service != nullptr);
        if(!service->started())
        {
            sight::service::get(sid)->start();
        }

        auto update_if_enabled = [sid]
                                 {
                                     if(auto action =
                                            std::dynamic_pointer_cast<sight::ui::action>(sight::service::get(sid));
                                        action != nullptr && action->enabled())
                                     {
                                         sight::service::get(sid)->update();
                                     }
                                 };
        QObject::connect(q_action, &QPushButton::clicked, update_if_enabled);
        q_action->setEnabled(service->enabled());
        auto is_enabled_slot = new_slot("setEnabledQt_" + action.sid, &QPushButton::setEnabled, q_action);
        is_enabled_slot->set_worker(worker());
        service->signal("isEnabled")->connect(is_enabled_slot);
        service->signal("isVisible")->connect(slot(slots::UPDATE_ACTIONS));
        q_action->setToolTip(QString::fromStdString(action.name));
        q_action->setIcon(
            QIcon(
                QString::fromStdString(
                    core::runtime::get_module_resource_file_path(
                        action.icon
                    ).string()
                )
            )
        );
        if(!action.shortcut.empty())
        {
            auto* shortcut = new QShortcut(QString::fromStdString(action.shortcut), m_speedDial->window());
            QObject::connect(shortcut, &QShortcut::activated, update_if_enabled);
            m_shortcuts.push_back(shortcut);
        }

        q_action->installEventFilter(new ResizeIconFilter);
        m_widgets.push_back(q_action);
    }

    m_speedDial->installEventFilter(new ResizeActionsFilter(m_widgets));
    updateActions();
    layout->addWidget(m_speedDial);
    qt_container->setLayout(layout);
    m_speedDial->show();
}

//------------------------------------------------------------------------------

void icon_speed_dial::updating()
{
}

//------------------------------------------------------------------------------

void icon_speed_dial::stopping()
{
    getContainer()->destroyContainer();
    m_speedDial = nullptr;
    std::ranges::for_each(m_shortcuts, [](QShortcut* _shortcut){_shortcut->setEnabled(false);});
    std::ranges::for_each(
        m_actions,
        [](const action& _a)
        {
            if(auto service = sight::service::get(_a.sid); service != nullptr && !service->stopped())
            {
                service->stop();
            }
        });
}

//------------------------------------------------------------------------------

void icon_speed_dial::updateActions()
{
    std::vector<QWidget*> actions;
    for(std::size_t i = 0 ; i < m_actions.size() ; i++)
    {
        if(auto action =
               std::dynamic_pointer_cast<sight::ui::action>(sight::service::get(m_actions[i].sid));
           action != nullptr && action->visible())
        {
            actions.push_back(m_widgets[i]);
        }
    }

    m_speedDial->updateActions(std::move(actions));
}

} // namespace sight::module::ui::qt
