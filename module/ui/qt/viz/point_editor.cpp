/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "module/ui/qt/viz/point_editor.hpp"

#include <core/base.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/map.hpp>
#include <data/string.hpp>

#include <ui/qt/container/widget.hpp>

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QSpacerItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::viz
{

static const core::com::slots::key_t GET_INTERACTION_SLOT = "get_interaction";

point_editor::point_editor() noexcept
{
    new_slot(GET_INTERACTION_SLOT, &point_editor::get_interaction, this);
}

//------------------------------------------------------------------------------

point_editor::~point_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void point_editor::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    auto* h_layout = new QHBoxLayout();

    auto* static_text_x = new QLabel(tr("x:"));
    h_layout->addWidget(static_text_x, 0, Qt::AlignVCenter);

    m_text_ctrl_x = new QLineEdit();
    m_text_ctrl_x->setValidator(new QDoubleValidator(m_text_ctrl_x));
    h_layout->addWidget(m_text_ctrl_x, 1, Qt::AlignVCenter);

    auto* static_text_y = new QLabel(tr("y:"));
    h_layout->addWidget(static_text_y, 0, Qt::AlignVCenter);

    m_text_ctrl_y = new QLineEdit();
    m_text_ctrl_y->setValidator(new QDoubleValidator(m_text_ctrl_y));
    h_layout->addWidget(m_text_ctrl_y, 1, Qt::AlignVCenter);

    auto* static_text_z = new QLabel(tr("z:"));
    h_layout->addWidget(static_text_z, 0, Qt::AlignVCenter);

    m_text_ctrl_z = new QLineEdit();
    m_text_ctrl_z->setValidator(new QDoubleValidator(m_text_ctrl_z));
    h_layout->addWidget(m_text_ctrl_z, 1, Qt::AlignVCenter);

    qt_container->set_layout(h_layout);
    this->updating();
}

//------------------------------------------------------------------------------

void point_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void point_editor::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void point_editor::updating()
{
}

//------------------------------------------------------------------------------

void point_editor::get_interaction(data::tools::picking_info _info)
{
    if(_info.m_event_id == data::tools::picking_info::event::mouse_left_down)
    {
        m_text_ctrl_x->setText(QString("%1").arg(_info.m_world_pos[0], 0, 'f', 3));
        m_text_ctrl_y->setText(QString("%1").arg(_info.m_world_pos[1], 0, 'f', 3));
        m_text_ctrl_z->setText(QString("%1").arg(_info.m_world_pos[2], 0, 'f', 3));
    }
}

//------------------------------------------------------------------------------

void point_editor::info(std::ostream& _sstream)
{
    _sstream << "Point Editor";
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
