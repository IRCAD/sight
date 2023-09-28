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

#include "SIconSpeedDial.hpp"

#include "core/thread/worker.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <service/op/Get.hpp>

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

    explicit ResizeActionsFilter(std::vector<QWidget*> actions) :
        m_actions(std::move(actions))
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*target*/, QEvent* e) override
    {
        if(e->type() == QEvent::Resize)
        {
            auto* resizeEvent = static_cast<QResizeEvent*>(e);
            std::ranges::for_each(m_actions, [resizeEvent](QWidget* w){w->setFixedSize(resizeEvent->size());});
        }

        return false;
    }
};

class ResizeIconFilter : public QObject
{
public:

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* target, QEvent* e) override
    {
        if(e->type() == QEvent::Resize)
        {
            auto* pushButton = qobject_cast<QPushButton*>(target);
            pushButton->setIconSize(pushButton->size());
        }

        return false;
    }
};

namespace sight::module::ui::qt
{

SIconSpeedDial::SIconSpeedDial()
{
    new_slot(SIconSpeedDial::slots::FOLD, [this]{m_speedDial->fold();});
    new_slot(SIconSpeedDial::slots::UNFOLD, [this]{m_speedDial->unfold();});
    new_slot(SIconSpeedDial::slots::UPDATE_ACTIONS, &SIconSpeedDial::updateActions, this);
}

//------------------------------------------------------------------------------

void SIconSpeedDial::configuring()
{
    initialize();

    const auto& config = this->getConfiguration();

    auto direction = config.get<std::string>("config.<xmlattr>.direction");
    if(direction == "up")
    {
        m_direction = sight::ui::qt::widget::SpeedDial::Direction::UP;
    }
    else if(direction == "right")
    {
        m_direction = sight::ui::qt::widget::SpeedDial::Direction::RIGHT;
    }
    else if(direction == "down")
    {
        m_direction = sight::ui::qt::widget::SpeedDial::Direction::DOWN;
    }
    else if(direction == "left")
    {
        m_direction = sight::ui::qt::widget::SpeedDial::Direction::LEFT;
    }
    else
    {
        SIGHT_ASSERT("Invalid direction " << direction << " for SpeedDial " << get_id(), false);
    }

    m_spacing           = config.get("config.<xmlattr>.spacing", -1);
    m_icon              = config.get<std::string>("config.<xmlattr>.icon");
    m_unfoldedIcon      = config.get("config.<xmlattr>.unfoldedIcon", "");
    m_animationDuration = config.get("config.<xmlattr>.animationDuration", -1);
    std::ranges::transform(
        boost::make_iterator_range(config.get_child("actions").equal_range("action")),
        std::back_inserter(m_actions),
        [](const auto& action) -> Action
        {
            return {
                .sid      = action.second.template get<std::string>("<xmlattr>.sid"),
                .name     = action.second.get("<xmlattr>.name", ""),
                .icon     = action.second.template get<std::string>("<xmlattr>.icon"),
                .shortcut = action.second.get("<xmlattr>.shortcut", "")
            };
        });
}

//------------------------------------------------------------------------------

void SIconSpeedDial::starting()
{
    create();

    const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    auto* layout     = new QBoxLayout(QBoxLayout::TopToBottom);
    m_speedDial = new sight::ui::qt::widget::SpeedDial(m_direction);
    m_speedDial->setObjectName(QString::fromStdString(serviceID));
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

    for(const Action& action : m_actions)
    {
        auto* qAction = new QPushButton;
        qAction->setFixedSize(m_speedDial->size());
        qAction->setObjectName(QString::fromStdString(action.name.empty() ? action.sid : action.name));
        std::string sid = action.sid;
        auto service    = std::dynamic_pointer_cast<sight::ui::action>(sight::service::get(sid));
        SIGHT_ASSERT("SIconSpeedDial only supports action", service != nullptr);
        if(!service->isStarted())
        {
            sight::service::get(sid)->start();
        }

        auto updateIfEnabled = [sid]
                               {
                                   if(auto action =
                                          std::dynamic_pointer_cast<sight::ui::action>(sight::service::get(sid));
                                      action != nullptr && action->enabled())
                                   {
                                       sight::service::get(sid)->update();
                                   }
                               };
        QObject::connect(qAction, &QPushButton::clicked, updateIfEnabled);
        qAction->setEnabled(service->enabled());
        auto isEnabledSlot = new_slot("setEnabledQt_" + action.sid, &QPushButton::setEnabled, qAction);
        isEnabledSlot->set_worker(worker());
        service->signal("isEnabled")->connect(isEnabledSlot);
        service->signal("isVisible")->connect(slot(slots::UPDATE_ACTIONS));
        qAction->setToolTip(QString::fromStdString(action.name));
        qAction->setIcon(
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
            QObject::connect(shortcut, &QShortcut::activated, updateIfEnabled);
            m_shortcuts.push_back(shortcut);
        }

        qAction->installEventFilter(new ResizeIconFilter);
        m_widgets.push_back(qAction);
    }

    m_speedDial->installEventFilter(new ResizeActionsFilter(m_widgets));
    updateActions();
    layout->addWidget(m_speedDial);
    qtContainer->setLayout(layout);
    m_speedDial->show();
}

//------------------------------------------------------------------------------

void SIconSpeedDial::updating()
{
}

//------------------------------------------------------------------------------

void SIconSpeedDial::stopping()
{
    getContainer()->destroyContainer();
    m_speedDial = nullptr;
    std::ranges::for_each(m_shortcuts, [](QShortcut* shortcut){shortcut->setEnabled(false);});
    std::ranges::for_each(
        m_actions,
        [](const Action& a)
        {
            if(auto service = sight::service::get(a.sid); service != nullptr && !service->isStopped())
            {
                service->stop();
            }
        });
}

//------------------------------------------------------------------------------

void SIconSpeedDial::updateActions()
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
