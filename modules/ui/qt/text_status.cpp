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

#include "text_status.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace sight::module::ui::qt
{

static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::slots::key_t SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::slots::key_t SET_STRING_PARAMETER_SLOT = "setStringParameter";

//-----------------------------------------------------------------------------

text_status::text_status() :
    m_labelStaticText(new QLabel())
{
    m_labelStaticText->setStyleSheet("font-weight: bold;");

    new_slot(SET_DOUBLE_PARAMETER_SLOT, &text_status::setDoubleParameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &text_status::setIntParameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &text_status::setBoolParameter, this);
    new_slot(SET_STRING_PARAMETER_SLOT, &text_status::setStringParameter, this);
}

//------------------------------------------------------------------------------

text_status::~text_status()
= default;

//------------------------------------------------------------------------------

void text_status::configuring()
{
    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    m_labelValue = new QLabel();
    m_labelValue->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    m_labelValue->setObjectName(service_id);

    this->initialize();

    const auto config = this->get_config();

    if(const auto label = config.get_optional<std::string>("label"); label.has_value())
    {
        const QString txt = QString::fromStdString(label.value());
        m_labelStaticText->setText(QString(txt + ": "));
    }

    QString color = "red";

    if(const auto color_cfg = config.get_optional<std::string>("color"); color_cfg.has_value())
    {
        const QString txt_color = QString::fromStdString(color_cfg.value());
        if(!txt_color.isEmpty())
        {
            color = txt_color;
        }
    }

    m_labelStaticText->setStyleSheet(m_labelStaticText->styleSheet() + " color: " + color + ";");
}

//------------------------------------------------------------------------------

void text_status::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* const layout = new QHBoxLayout();
    layout->addWidget(m_labelStaticText);
    layout->addWidget(m_labelValue);

    layout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    qt_container->setLayout(layout);

    // Get input data.
    const auto string_input = m_string.lock();

    if(string_input)
    {
        m_labelValue->setText(QString::fromStdString(string_input->value()));
    }
}

//------------------------------------------------------------------------------

service::connections_t text_status::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_STRING_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void text_status::updating()
{
    // Get input data.
    const auto string_input = m_string.lock();

    if(string_input)
    {
        m_labelValue->setText(QString::fromStdString(string_input->value()));
    }
}

//------------------------------------------------------------------------------

void text_status::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void text_status::setIntParameter(int _val)
{
    QString str;
    str.setNum(_val);
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void text_status::setDoubleParameter(double _val)
{
    QString str;
    str.setNum(_val, 'g', 8); // 8 decimals precision
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void text_status::setBoolParameter(bool _val)
{
    QString str(_val ? "ON" : "OFF");
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void text_status::setStringParameter(std::string _val)
{
    m_labelValue->setText(QString::fromStdString(_val));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
