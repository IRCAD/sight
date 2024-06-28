/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <ui/qt/container/widget.hpp>

#include <boost/range/iterator_range.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QVariant>
#include <QVBoxLayout>

namespace sight::module::ui::qt
{

enum class status_color : std::size_t
{
    green,
    orange,
    red
};

//------------------------------------------------------------------------------

QIcon& icon(status_color _color)
{
    std::array icons = {"status_green.svg", "status_orange.svg", "status_red.svg"};
    const auto path  =
        core::runtime::get_module_resource_file_path(
            "sight::module::ui::icons",
            icons[static_cast<std::size_t>(_color)]
        );

    static QIcon icon;
    icon.addFile(QString::fromStdString(path.string()), QSize(), QIcon::Disabled);

    return icon;
}

//-----------------------------------------------------------------------------

status::status() noexcept
{
    new_slot(slots::CHANGE_TO_GREEN_SLOT, &status::change_to_green, this);
    new_slot(slots::CHANGE_TO_RED_SLOT, &status::change_to_red, this);
    new_slot(slots::CHANGE_TO_ORANGE_SLOT, &status::change_to_orange, this);
    new_slot(slots::TOGGLE_GREEN_RED_SLOT, &status::toggle_green_red, this);
    new_slot(slots::CHANGE_NTH_TO_GREEN_SLOT, &status::change_nth_to_green, this);
    new_slot(slots::CHANGE_NTH_TO_RED_SLOT, &status::change_nth_to_red, this);
    new_slot(slots::CHANGE_NTH_TO_ORANGE_SLOT, &status::change_nth_to_orange, this);
    new_slot(slots::TOGGLE_NTH_GREEN_RED_SLOT, &status::toggle_nth_green_red, this);
}

//------------------------------------------------------------------------------

void status::configuring(const config_t& _config)
{
    this->initialize();

    m_green_tooltip  = _config.get<std::string>("green", "");
    m_red_tooltip    = _config.get<std::string>("red", "");
    m_orange_tooltip = _config.get<std::string>("orange", "");
    m_orientation    = _config.get<std::string>("layout", "horizontal");
    SIGHT_ASSERT(
        "Value for element 'layout' should be 'horizontal' or 'vertical'.",
        m_orientation == "horizontal" || m_orientation == "vertical"
    );

    const auto label_display = _config.get<std::string>("labels.<xmlattr>.display", "under");

    SIGHT_ASSERT(
        "Value for element 'display' should be 'beside' or 'under'.",
        label_display == "beside" || label_display == "under"
    );
    m_label_display = label_display == "beside" ? label_display::BESIDE : label_display::UNDER;

    const auto config_labels = _config.get_child_optional("labels");

    // Check if the labels tag exists
    if(config_labels)
    {
        const auto label_status_config = config_labels.get().equal_range("name");

        // Fill the label_status vector
        for(int i = 0 ; const service::config_t::value_type& v : boost::make_iterator_range(label_status_config))
        {
            const auto label = v.second.get<std::string>("");
            auto* status     = new QToolButton();
            status->setText(QString::fromStdString(label));

            const QString service_id = QString::fromStdString(base_id());
            status->setObjectName(service_id + "/" + QString::number(i++));

            m_label_status.push_back(status);
        }
    }
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

    for(auto& status : m_label_status)
    {
        if(!status->text().isEmpty())
        {
            if(m_label_display == label_display::BESIDE)
            {
                status->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }
            else
            {
                status->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            }
        }
        else
        {
            status->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }

        layout->addWidget(status);
        status->setDisabled(true);
        status->setProperty("class", "status");
    }

    layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(layout);

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
}

//------------------------------------------------------------------------------

void status::change_to_green()
{
    for(auto& status : m_label_status)
    {
        status->setIcon(icon(status_color::green));
        status->setToolTip(QString::fromStdString(m_green_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_to_red()
{
    for(auto& status : m_label_status)
    {
        status->setIcon(icon(status_color::red));
        status->setToolTip(QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_to_orange()
{
    for(auto& status : m_label_status)
    {
        status->setIcon(icon(status_color::orange));
        status->setToolTip(QString::fromStdString(m_orange_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggle_green_red(const bool _green)
{
    for(auto& status : m_label_status)
    {
        status->setIcon(_green ? icon(status_color::green) : icon(status_color::red));
        status->setToolTip(_green ? QString::fromStdString(m_green_tooltip) : QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_green(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_label_status.size() - 1 << "]",
        _index < 0 || _index >= int(m_label_status.size())
    );

    if(_index >= 0 && _index < int(m_label_status.size()))
    {
        const auto status = m_label_status.at(_index);
        status->setIcon(icon(status_color::green));
        status->setToolTip(QString::fromStdString(m_green_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_red(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_label_status.size() - 1 << "]",
        _index < 0 || _index >= int(m_label_status.size())
    );

    if(_index >= 0 && _index < int(m_label_status.size()))
    {
        const auto status = m_label_status.at(_index);
        status->setIcon(icon(status_color::red));
        status->setToolTip(QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_orange(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_label_status.size() - 1 << "]",
        _index < 0 || _index >= int(m_label_status.size())
    );

    if(_index >= 0 && _index < int(m_label_status.size()))
    {
        const auto status = m_label_status.at(_index);
        status->setIcon(icon(status_color::orange));
        status->setToolTip(QString::fromStdString(m_orange_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggle_nth_green_red(const int _index, const bool _green)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_label_status.size() - 1 << "]",
        _index < 0 || _index >= int(m_label_status.size())
    );

    if(_index >= 0 && _index < int(m_label_status.size()))
    {
        const auto status = m_label_status.at(_index);
        status->setIcon(_green ? icon(status_color::green) : icon(status_color::red));
        status->setToolTip(_green ? QString::fromStdString(m_green_tooltip) : QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
