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

#include "SText.hpp"

#include <core/com/Slots.hxx>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace sight::module::ui::qt
{

//-----------------------------------------------------------------------------

SText::SText() :
    m_label(new QLabel())
{
    m_label->setStyleSheet("font-weight: bold;");
}

//------------------------------------------------------------------------------

void SText::configuring()
{
    this->initialize();

    const auto config = this->getConfiguration();

    if(const auto label = config.get_optional<std::string>("label"); label.has_value())
    {
        const QString txt = QString::fromStdString(label.value());
        m_label->setText(QString(txt));
    }

    QString color = "white";

    if(const auto colorCfg = config.get_optional<std::string>("color"); colorCfg.has_value())
    {
        const QString txtColor = QString::fromStdString(colorCfg.value());
        if(!txtColor.isEmpty())
        {
            color = txtColor;
        }
    }

    m_label->setStyleSheet(m_label->styleSheet() + " color: " + color + ";");
}

//------------------------------------------------------------------------------

void SText::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    auto* const layout = new QHBoxLayout();
    layout->addWidget(m_label);

    layout->setAlignment(Qt::AlignCenter);

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SText::updating()
{
}

//------------------------------------------------------------------------------

void SText::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
