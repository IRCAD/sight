/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

namespace sight::module::ui::qt
{

static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT = "set_double_parameter";
static const core::com::slots::key_t SET_INT_PARAMETER_SLOT    = "set_int_parameter";
static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT   = "set_bool_parameter";
static const core::com::slots::key_t SET_STRING_PARAMETER_SLOT = "set_string_parameter";

//-----------------------------------------------------------------------------

text_status::text_status() :
    m_label_static_text(new QLabel())
{
    m_label_static_text->setStyleSheet("font-weight: bold;");

    new_slot(SET_DOUBLE_PARAMETER_SLOT, &text_status::set_double_parameter, this);
    new_slot(SET_INT_PARAMETER_SLOT, &text_status::set_int_parameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &text_status::set_bool_parameter, this);
    new_slot(SET_STRING_PARAMETER_SLOT, &text_status::set_string_parameter, this);
}

//------------------------------------------------------------------------------

text_status::~text_status()
= default;

//------------------------------------------------------------------------------

void text_status::configuring()
{
    const QString service_id = QString::fromStdString(base_id());

    m_label_value = new QLabel();
    m_label_value->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    m_label_value->setObjectName(service_id);

    this->initialize();

    const auto config = this->get_config();

    if(const auto label = config.get_optional<std::string>("label"); label.has_value())
    {
        const QString txt = QString::fromStdString(label.value());
        m_label_static_text->setText(QString(txt + ": "));
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

    m_label_static_text->setStyleSheet(m_label_static_text->styleSheet() + " color: " + color + ";");
}

//------------------------------------------------------------------------------

void text_status::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* const layout = new QHBoxLayout();
    layout->addWidget(m_label_static_text);
    layout->addWidget(m_label_value);

    layout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    qt_container->set_layout(layout);

    // Get input data.
    const auto string_input = m_string.lock();

    if(string_input)
    {
        m_label_value->setText(QString::fromStdString(string_input->value()));
    }
}

//------------------------------------------------------------------------------

service::connections_t text_status::auto_connections() const
{
    service::connections_t connections;
    connections.push(STRING_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void text_status::updating()
{
    // Get input data.
    const auto string_input = m_string.lock();

    if(string_input)
    {
        m_label_value->setText(QString::fromStdString(string_input->value()));
    }
}

//------------------------------------------------------------------------------

void text_status::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void text_status::set_int_parameter(int _val)
{
    QString str;
    str.setNum(_val);
    m_label_value->setText(str);
}

//------------------------------------------------------------------------------

void text_status::set_double_parameter(double _val)
{
    QString str;
    str.setNum(_val, 'g', 8); // 8 decimals precision
    m_label_value->setText(str);
}

//------------------------------------------------------------------------------

void text_status::set_bool_parameter(bool _val)
{
    QString str(_val ? "ON" : "OFF");
    m_label_value->setText(str);
}

//------------------------------------------------------------------------------

void text_status::set_string_parameter(std::string _val)
{
    m_label_value->setText(QString::fromStdString(_val));
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
