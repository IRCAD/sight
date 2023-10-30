/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "status.hpp"

#include <core/base.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <boost/foreach.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QPalette>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

namespace sight::module::ui::qt
{

const core::com::slots::key_t status::CHANGE_TO_GREEN_SLOT      = "changeToGreen";
const core::com::slots::key_t status::CHANGE_TO_RED_SLOT        = "changeToRed";
const core::com::slots::key_t status::CHANGE_TO_ORANGE_SLOT     = "changeToOrange";
const core::com::slots::key_t status::TOGGLE_GREEN_RED_SLOT     = "toggleGreenRed";
const core::com::slots::key_t status::CHANGE_NTH_TO_GREEN_SLOT  = "changeNthToGreen";
const core::com::slots::key_t status::CHANGE_NTH_TO_RED_SLOT    = "changeNthToRed";
const core::com::slots::key_t status::CHANGE_NTH_TO_ORANGE_SLOT = "changeNthToOrange";
const core::com::slots::key_t status::TOGGLE_NTH_GREEN_RED_SLOT = "toggleNthGreenRed";

//-----------------------------------------------------------------------------

status::status() noexcept
{
    new_slot(CHANGE_TO_GREEN_SLOT, &status::change_to_green, this);
    new_slot(CHANGE_TO_RED_SLOT, &status::change_to_red, this);
    new_slot(CHANGE_TO_ORANGE_SLOT, &status::change_to_orange, this);
    new_slot(TOGGLE_GREEN_RED_SLOT, &status::toggle_green_red, this);
    new_slot(CHANGE_NTH_TO_GREEN_SLOT, &status::change_nth_to_green, this);
    new_slot(CHANGE_NTH_TO_RED_SLOT, &status::change_nth_to_red, this);
    new_slot(CHANGE_NTH_TO_ORANGE_SLOT, &status::change_nth_to_orange, this);
    new_slot(TOGGLE_NTH_GREEN_RED_SLOT, &status::toggle_nth_green_red, this);
}

//------------------------------------------------------------------------------

status::~status() noexcept =
    default;

//------------------------------------------------------------------------------

void status::starting()
{
    this->create();

    auto qt_container  = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QBoxLayout* layout = nullptr;
    if(m_layout == "horizontal")
    {
        layout = new QHBoxLayout();
    }
    else
    {
        layout = new QVBoxLayout();
    }

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    for(std::size_t i = 0 ; i < m_count ; ++i)
    {
        QPointer<QLabel> indicator = new QLabel();
        indicator->setObjectName(service_id + "/" + QString::number(i));
        indicator->setFixedWidth(static_cast<int>(m_width));
        indicator->setFixedHeight(static_cast<int>(m_height));
        m_indicator.push_back(indicator);

        layout->addWidget(indicator);
        layout->addWidget(m_label_status.at(static_cast<int>(i)));
    }

    qt_container->set_layout(layout);

    this->change_to_red();
}

//------------------------------------------------------------------------------

void status::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void status::configuring()
{
    this->initialize();

    const auto config = this->get_config();

    m_green_tooltip  = config.get<std::string>("green", "");
    m_red_tooltip    = config.get<std::string>("red", "");
    m_orange_tooltip = config.get<std::string>("orange", "");
    m_layout         = config.get<std::string>("layout", "horizontal");
    SIGHT_ASSERT(
        "Value for element 'layout' should be 'horizontal' or 'vertical'.",
        m_layout == "horizontal" || m_layout == "vertical"
    );

    const std::string form = config.get<std::string>("form", "square");
    SIGHT_ASSERT("Value for element 'form' should be 'circle' or 'square'.", form == "circle" || form == "square");
    m_is_circular = (form == "circle");

    m_width  = config.get<std::size_t>("size.width", 20);
    m_height = config.get<std::size_t>("size.height", 20);

    m_border_radius = m_is_circular ? QString::number(std::min(m_width, m_height) / 2) + "px" : "0";

    m_count = config.get<std::size_t>("count", 1);

    if(m_count == 1)
    {
        const std::string label = config.get<std::string>("labelStatus", "");
        QPointer<QLabel> q_lab  = new QLabel();
        q_lab->setText(QString::fromStdString(label));
        m_label_status.push_back(q_lab);
    }
    else
    {
        const auto config_labels = config.get_child_optional("labels");
        // Check if the labels tag exists
        if(config_labels.is_initialized())
        {
            const std::size_t count_label_status = config_labels.get().count("labelStatus");

            SIGHT_WARN_IF(
                "Number of 'labelStatus' (" << count_label_status << ") is different from needed status (" << m_count << ").",
                count_label_status != m_count
            );
            SIGHT_WARN_IF(
                "'labelStatus' from " << m_count + 1 << " to " << count_label_status << " will be lost.",
                count_label_status > m_count
            );

            const auto label_status_config = config_labels.get().equal_range("labelStatus");
            // Fill the labelStatus vector
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const service::config_t::value_type& v, label_status_config)
            {
                const auto label       = v.second.get<std::string>("");
                QPointer<QLabel> q_lab = new QLabel();
                q_lab->setText(QString::fromStdString(label));

                m_label_status.push_back(q_lab);
            }

            // If there is less labelStatus than needed ones, fill with empty strings
            if(count_label_status < m_count)
            {
                SIGHT_WARN("'labelStatus' from " << count_label_status + 1 << " to " << m_count << " will be empty.");
                for(std::size_t i = count_label_status ; i < m_count ; ++i)
                {
                    QPointer<QLabel> q_lab = new QLabel();
                    q_lab->setText("");
                    m_label_status.push_back(q_lab);
                }
            }
        }
        else
        {
            for(std::size_t i = 0 ; i < m_count ; ++i)
            {
                QPointer<QLabel> q_lab = new QLabel();
                q_lab->setText("");
                m_label_status.push_back(q_lab);
            }
        }
    }
}

//------------------------------------------------------------------------------

void status::updating()
{
}

//------------------------------------------------------------------------------

void status::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void status::change_to_green()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: green; border-radius: " + m_border_radius + ";");
        it->setToolTip(QString::fromStdString(m_green_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_to_red()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: red; border-radius: " + m_border_radius + ";");
        it->setToolTip(QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_to_orange()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: orange; border-radius: " + m_border_radius + ";");
        it->setToolTip(QString::fromStdString(m_orange_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggle_green_red(const bool _green)
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet(
            "background-color:" + QString(_green ? "green" : "red") + "; border-radius: " + m_border_radius + ";"
        );
        it->setToolTip(_green ? QString::fromStdString(m_green_tooltip) : QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_green(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        _index < 0 || _index >= int(m_count)
    );

    if(_index >= 0 && _index < int(m_count))
    {
        m_indicator.at(_index)->setStyleSheet("background-color: green; border-radius: " + m_border_radius + ";");
        m_indicator.at(_index)->setToolTip(QString::fromStdString(m_green_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_red(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        _index < 0 || _index >= int(m_count)
    );

    if(_index >= 0 && _index < int(m_count))
    {
        m_indicator.at(_index)->setStyleSheet("background-color: red; border-radius: " + m_border_radius + ";");
        m_indicator.at(_index)->setToolTip(QString::fromStdString(m_red_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::change_nth_to_orange(const int _index)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        _index < 0 || _index >= int(m_count)
    );

    if(_index >= 0 && _index < int(m_count))
    {
        m_indicator.at(_index)->setStyleSheet("background-color: orange; border-radius: " + m_border_radius + ";");
        m_indicator.at(_index)->setToolTip(QString::fromStdString(m_orange_tooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggle_nth_green_red(const int _index, const bool _green)
{
    SIGHT_DEBUG_IF(
        "Index(" << _index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        _index < 0 || _index >= int(m_count)
    );

    if(_index >= 0 && _index < int(m_count))
    {
        m_indicator.at(_index)->setStyleSheet(
            "background-color:" + QString(_green ? "green" : "red")
            + "; border-radius: " + m_border_radius + ";"
        );
        m_indicator.at(_index)->setToolTip(
            _green ? QString::fromStdString(m_green_tooltip)
                   : QString::fromStdString(m_red_tooltip)
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
