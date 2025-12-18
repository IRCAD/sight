/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include "progress_bar.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <ui/qt/container/widget.hpp>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

progress_bar::progress_bar() noexcept
{
    new_slot(slots::ADD_MONITOR, &progress_bar::add_monitor, this);
    new_signal<signals::finished_t>(signals::FINISHED);
}

//-----------------------------------------------------------------------------

void progress_bar::starting()
{
    this->create();

    const auto widget_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    auto* const qt_container    = widget_container->get_qt_container();

    const auto service_id = QString::fromStdString(base_id());
    qt_container->setObjectName(service_id);
    qt_container->setContentsMargins(0, 0, 0, 0);

    // Get the progress bar configuration.
    const auto& config = this->get_config();

    const auto show_title  = config.get<bool>("config.<xmlattr>.show_title", true);
    const auto show_cancel = config.get<bool>("config.<xmlattr>.show_cancel", true);
    const auto svg_path    = config.get_optional<std::filesystem::path>("config.<xmlattr>.svg");
    const auto pulse       = config.get<bool>("config.<xmlattr>.pulse", svg_path && !svg_path->empty());

    const auto svg_size = config.get_optional<int>("config.<xmlattr>.svg_size");

    if(svg_size)
    {
        SIGHT_ASSERT("The svg size attribute is only valid if an svg file is used.", svg_path && !svg_path->empty());
    }

    const auto weak_this = this->weak_from_this();

    // Create the progress bar widget.
    m_progress_bar_widget = std::make_shared<sight::ui::qt::widget::progress_bar>(
        qt_container,
        show_title,
        show_cancel,
        pulse,
        svg_path ? std::make_optional(*svg_path) : std::nullopt,
        svg_size ? std::make_optional(*svg_size) : std::nullopt,
        [weak_this]
        {
            if(auto shared_this = dynamic_pointer_cast<progress_bar>(weak_this.lock()); shared_this)
            {
                shared_this->async_emit(signals::FINISHED);
            }
        });

    // Create the layout.
    auto* const layout = new QHBoxLayout();
    layout->setObjectName(service_id + QString::fromLatin1("/QHBoxLayout"));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_progress_bar_widget->widget());

    // Add layout to the qt_container.
    widget_container->set_layout(layout);
}

//-----------------------------------------------------------------------------

void progress_bar::stopping()
{
    m_progress_bar_widget.reset();

    this->destroy();
} // namespace sight::module::ui::qt

//-----------------------------------------------------------------------------

void progress_bar::updating()
{
}

//-----------------------------------------------------------------------------

void progress_bar::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void progress_bar::add_monitor(core::progress::monitor::sptr _monitor)
{
    if(m_progress_bar_widget)
    {
        m_progress_bar_widget->add_monitor(_monitor);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt
