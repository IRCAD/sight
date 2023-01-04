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

#include "STextStatus.hpp"

#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace sight::module::ui::qt
{

static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::Slots::SlotKeyType s_SET_STRING_PARAMETER_SLOT = "setStringParameter";

//-----------------------------------------------------------------------------

STextStatus::STextStatus() :
    m_labelStaticText(new QLabel())
{
    m_labelStaticText->setStyleSheet("font-weight: bold;");

    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &STextStatus::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &STextStatus::setIntParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &STextStatus::setBoolParameter, this);
    newSlot(s_SET_STRING_PARAMETER_SLOT, &STextStatus::setStringParameter, this);
}

//------------------------------------------------------------------------------

STextStatus::~STextStatus()
= default;

//------------------------------------------------------------------------------

void STextStatus::configuring()
{
    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    m_labelValue = new QLabel();
    m_labelValue->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    m_labelValue->setObjectName(serviceID);

    this->initialize();

    const auto config = this->getConfiguration();

    if(const auto label = config.get_optional<std::string>("label"); label.has_value())
    {
        const QString txt = QString::fromStdString(label.value());
        m_labelStaticText->setText(QString(txt + ": "));
    }

    QString color = "red";

    if(const auto colorCfg = config.get_optional<std::string>("color"); colorCfg.has_value())
    {
        const QString txtColor = QString::fromStdString(colorCfg.value());
        if(!txtColor.isEmpty())
        {
            color = txtColor;
        }
    }

    m_labelStaticText->setStyleSheet(m_labelStaticText->styleSheet() + " color: " + color + ";");
}

//------------------------------------------------------------------------------

void STextStatus::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    auto* const layout = new QHBoxLayout();
    layout->addWidget(m_labelStaticText);
    layout->addWidget(m_labelValue);

    layout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    qtContainer->setLayout(layout);

    // Get input data.
    const auto stringInput = m_string.lock();

    if(stringInput)
    {
        m_labelValue->setText(QString::fromStdString(stringInput->value()));
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap STextStatus::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_STRING_INPUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void STextStatus::updating()
{
    // Get input data.
    const auto stringInput = m_string.lock();

    if(stringInput)
    {
        m_labelValue->setText(QString::fromStdString(stringInput->value()));
    }
}

//------------------------------------------------------------------------------

void STextStatus::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void STextStatus::setIntParameter(int _val)
{
    QString str;
    str.setNum(_val);
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setDoubleParameter(double _val)
{
    QString str;
    str.setNum(_val, 'g', 8); // 8 decimals precision
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setBoolParameter(bool _val)
{
    QString str(_val ? "ON" : "OFF");
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setStringParameter(std::string _val)
{
    m_labelValue->setText(QString::fromStdString(_val));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
