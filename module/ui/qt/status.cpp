/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "status.hpp"

#include "service/base.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <ui/qt/container/widget.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QVariant>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

status::status() noexcept
{
    new_slot(slots::CHANGE_TO_GREEN_SLOT, &status::change_to_green, this);
    new_slot(slots::CHANGE_TO_RED_SLOT, &status::change_to_red, this);
    new_slot(slots::CHANGE_TO_ORANGE_SLOT, &status::change_to_orange, this);
    new_slot(slots::TOGGLE_GREEN_RED_SLOT, &status::toggle_green_red, this);
}

//------------------------------------------------------------------------------

service::connections_t status::auto_connections() const
{
    return {
        {m_color, sight::data::signals::MODIFIED, base::slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void status::configuring(const config_t& _config)
{
    this->initialize();

    m_label       = _config.get<std::string>("label", "");
    m_orientation = _config.get<std::string>("layout", "horizontal");
    SIGHT_ASSERT(
        "Value for element 'layout' should be 'horizontal' or 'vertical'.",
        m_orientation == "horizontal" || m_orientation == "vertical"
    );
}

//------------------------------------------------------------------------------

void status::starting()
{
    this->create();

    auto qt_container  = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QBoxLayout* layout = nullptr;
    if(m_orientation == "horizontal")
    {
        layout = new QHBoxLayout();
    }
    else
    {
        layout = new QVBoxLayout();
    }

    m_status_button = new QToolButton();
    m_status_button->setProperty("class", "status");
    // Keep the button visually enabled (no gray icon), but ignore user mouse interaction.
    m_status_button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_status_button->setFocusPolicy(Qt::NoFocus);
    const QString service_id = QString::fromStdString(base_id());
    m_status_button->setObjectName(service_id + "/status_button");

    if(!m_label.empty())
    {
        m_status_button->setText(QString::fromStdString(m_label));

        if(m_label_display == label_display::beside)
        {
            m_status_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        }
        else
        {
            m_status_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
    }
    else
    {
        m_status_button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    layout->addWidget(m_status_button);
    layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(layout);

    // Initialize with red color
    this->change_to_red();
}

//------------------------------------------------------------------------------

void status::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void status::updating()
{
    update_circle_color();
}

//------------------------------------------------------------------------------

void status::update_circle_color()
{
    if(m_status_button.isNull())
    {
        return;
    }

    const auto& color = *m_color;

    // This function is not that slow (0,04ms measured in debug), but it is called at each update,
    // so we want to avoid doing unnecessary updates when the color has not changed.
    if(color == m_prev_color_value)
    {
        return;
    }

    m_prev_color_value = color;

    // Create a pixmap with a colored circle
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto qcolor = QColor::fromRgbF(color[0], color[1], color[2], color[3]);

    painter.setBrush(QBrush(qcolor));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, 28, 28);

    m_status_button->setIcon(QIcon(pixmap));
}

//------------------------------------------------------------------------------

void status::change_to_green()
{
    *m_color.lock() = sight::data::color(0.0F, 1.0F, 0.0F, 1.0F);
    m_status_button->setToolTip("Green"); // Backward compatible, used in unit-tests
    update_circle_color();
}

//------------------------------------------------------------------------------

void status::change_to_red()
{
    *m_color.lock() = sight::data::color(1.0F, 0.0F, 0.0F, 1.0F);
    m_status_button->setToolTip("Red"); // Backward compatible, used in unit-tests
    update_circle_color();
}

//------------------------------------------------------------------------------

void status::change_to_orange()
{
    *m_color.lock() = sight::data::color(1.0F, 0.647F, 0.0F, 1.0F);
    m_status_button->setToolTip("Orange"); // Backward compatible, used in unit-tests
    update_circle_color();
}

//------------------------------------------------------------------------------

void status::toggle_green_red(const bool _green)
{
    if(_green)
    {
        this->change_to_green();
    }
    else
    {
        this->change_to_red();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
