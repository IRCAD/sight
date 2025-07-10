/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "text.hpp"

#include <core/com/slots.hxx>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

text::text() :
    m_label(new QLabel())
{
    m_label->setStyleSheet("font-weight: bold;");
}

//------------------------------------------------------------------------------

void text::configuring()
{
    this->initialize();

    const auto config = this->get_config();

    QString style_sheet;

    if(const auto label = config.get_optional<std::string>("label"); label.has_value())
    {
        const QString txt = QString::fromStdString(label.value());
        m_label->setText(QString(txt));
    }

    QString color = "white";

    if(const auto color_cfg = config.get_optional<std::string>("color"); color_cfg.has_value())
    {
        const QString txt_color = QString::fromStdString(color_cfg.value());
        if(!txt_color.isEmpty())
        {
            color = txt_color;
        }
    }

    const auto size   = config.get<std::string>("size", "14pt");
    const auto weight = config.get<std::string>("weight", "bold");

    m_label->setStyleSheet(
        m_label->styleSheet() + " color: " + color + ";" + "font-size: "
        + QString::fromStdString(size) + ";" + "font-weight: " + QString::fromStdString(weight) + ";"
    );
}

//------------------------------------------------------------------------------

void text::starting()
{
    this->create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* const layout = new QHBoxLayout();
    layout->addWidget(m_label);

    layout->setAlignment(Qt::AlignCenter);

    qt_container->set_layout(layout);
}

//------------------------------------------------------------------------------

void text::updating()
{
}

//------------------------------------------------------------------------------

void text::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
