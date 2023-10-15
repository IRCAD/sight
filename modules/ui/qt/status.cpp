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
    new_slot(CHANGE_TO_GREEN_SLOT, &status::changeToGreen, this);
    new_slot(CHANGE_TO_RED_SLOT, &status::changeToRed, this);
    new_slot(CHANGE_TO_ORANGE_SLOT, &status::changeToOrange, this);
    new_slot(TOGGLE_GREEN_RED_SLOT, &status::toggleGreenRed, this);
    new_slot(CHANGE_NTH_TO_GREEN_SLOT, &status::changeNthToGreen, this);
    new_slot(CHANGE_NTH_TO_RED_SLOT, &status::changeNthToRed, this);
    new_slot(CHANGE_NTH_TO_ORANGE_SLOT, &status::changeNthToOrange, this);
    new_slot(TOGGLE_NTH_GREEN_RED_SLOT, &status::toggleNthGreenRed, this);
}

//------------------------------------------------------------------------------

status::~status() noexcept =
    default;

//------------------------------------------------------------------------------

void status::starting()
{
    this->create();

    auto qtContainer   = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    QBoxLayout* layout = nullptr;
    if(m_layout == "horizontal")
    {
        layout = new QHBoxLayout();
    }
    else
    {
        layout = new QVBoxLayout();
    }

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    for(std::size_t i = 0 ; i < m_count ; ++i)
    {
        QPointer<QLabel> indicator = new QLabel();
        indicator->setObjectName(serviceID + "/" + QString::number(i));
        indicator->setFixedWidth(static_cast<int>(m_width));
        indicator->setFixedHeight(static_cast<int>(m_height));
        m_indicator.push_back(indicator);

        layout->addWidget(indicator);
        layout->addWidget(m_labelStatus.at(static_cast<int>(i)));
    }

    qtContainer->setLayout(layout);

    this->changeToRed();
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

    m_greenTooltip  = config.get<std::string>("green", "");
    m_redTooltip    = config.get<std::string>("red", "");
    m_orangeTooltip = config.get<std::string>("orange", "");
    m_layout        = config.get<std::string>("layout", "horizontal");
    SIGHT_ASSERT(
        "Value for element 'layout' should be 'horizontal' or 'vertical'.",
        m_layout == "horizontal" || m_layout == "vertical"
    );

    const std::string form = config.get<std::string>("form", "square");
    SIGHT_ASSERT("Value for element 'form' should be 'circle' or 'square'.", form == "circle" || form == "square");
    m_isCircular = (form == "circle");

    m_width  = config.get<std::size_t>("size.width", 20);
    m_height = config.get<std::size_t>("size.height", 20);

    m_borderRadius = m_isCircular ? QString::number(std::min(m_width, m_height) / 2) + "px" : "0";

    m_count = config.get<std::size_t>("count", 1);

    if(m_count == 1)
    {
        const std::string label = config.get<std::string>("labelStatus", "");
        QPointer<QLabel> qLab   = new QLabel();
        qLab->setText(QString::fromStdString(label));
        m_labelStatus.push_back(qLab);
    }
    else
    {
        const auto configLabels = config.get_child_optional("labels");
        // Check if the labels tag exists
        if(configLabels.is_initialized())
        {
            const std::size_t countLabelStatus = configLabels.get().count("labelStatus");

            SIGHT_WARN_IF(
                "Number of 'labelStatus' (" << countLabelStatus << ") is different from needed status (" << m_count << ").",
                countLabelStatus != m_count
            );
            SIGHT_WARN_IF(
                "'labelStatus' from " << m_count + 1 << " to " << countLabelStatus << " will be lost.",
                countLabelStatus > m_count
            );

            const auto labelStatusConfig = configLabels.get().equal_range("labelStatus");
            // Fill the labelStatus vector
            // NOLINTNEXTLINE(bugprone-branch-clone)
            BOOST_FOREACH(const service::config_t::value_type& v, labelStatusConfig)
            {
                const auto label      = v.second.get<std::string>("");
                QPointer<QLabel> qLab = new QLabel();
                qLab->setText(QString::fromStdString(label));

                m_labelStatus.push_back(qLab);
            }

            // If there is less labelStatus than needed ones, fill with empty strings
            if(countLabelStatus < m_count)
            {
                SIGHT_WARN("'labelStatus' from " << countLabelStatus + 1 << " to " << m_count << " will be empty.");
                for(std::size_t i = countLabelStatus ; i < m_count ; ++i)
                {
                    QPointer<QLabel> qLab = new QLabel();
                    qLab->setText("");
                    m_labelStatus.push_back(qLab);
                }
            }
        }
        else
        {
            for(std::size_t i = 0 ; i < m_count ; ++i)
            {
                QPointer<QLabel> qLab = new QLabel();
                qLab->setText("");
                m_labelStatus.push_back(qLab);
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

void status::changeToGreen()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: green; border-radius: " + m_borderRadius + ";");
        it->setToolTip(QString::fromStdString(m_greenTooltip));
    }
}

//------------------------------------------------------------------------------

void status::changeToRed()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: red; border-radius: " + m_borderRadius + ";");
        it->setToolTip(QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void status::changeToOrange()
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet("background-color: orange; border-radius: " + m_borderRadius + ";");
        it->setToolTip(QString::fromStdString(m_orangeTooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggleGreenRed(const bool green)
{
    for(auto& it : m_indicator)
    {
        it->setStyleSheet(
            "background-color:" + QString(green ? "green" : "red") + "; border-radius: " + m_borderRadius + ";"
        );
        it->setToolTip(green ? QString::fromStdString(m_greenTooltip) : QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void status::changeNthToGreen(const int index)
{
    SIGHT_DEBUG_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        index < 0 || index >= int(m_count)
    );

    if(index >= 0 && index < int(m_count))
    {
        m_indicator.at(index)->setStyleSheet("background-color: green; border-radius: " + m_borderRadius + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_greenTooltip));
    }
}

//------------------------------------------------------------------------------

void status::changeNthToRed(const int index)
{
    SIGHT_DEBUG_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        index < 0 || index >= int(m_count)
    );

    if(index >= 0 && index < int(m_count))
    {
        m_indicator.at(index)->setStyleSheet("background-color: red; border-radius: " + m_borderRadius + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_redTooltip));
    }
}

//------------------------------------------------------------------------------

void status::changeNthToOrange(const int index)
{
    SIGHT_DEBUG_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        index < 0 || index >= int(m_count)
    );

    if(index >= 0 && index < int(m_count))
    {
        m_indicator.at(index)->setStyleSheet("background-color: orange; border-radius: " + m_borderRadius + ";");
        m_indicator.at(index)->setToolTip(QString::fromStdString(m_orangeTooltip));
    }
}

//------------------------------------------------------------------------------

void status::toggleNthGreenRed(const int index, const bool green)
{
    SIGHT_DEBUG_IF(
        "Index(" << index << ") must be in vector range [0:" << m_indicator.size() - 1 << "]",
        index < 0 || index >= int(m_count)
    );

    if(index >= 0 && index < int(m_count))
    {
        m_indicator.at(index)->setStyleSheet(
            "background-color:" + QString(green ? "green" : "red")
            + "; border-radius: " + m_borderRadius + ";"
        );
        m_indicator.at(index)->setToolTip(
            green ? QString::fromStdString(m_greenTooltip)
                  : QString::fromStdString(m_redTooltip)
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
