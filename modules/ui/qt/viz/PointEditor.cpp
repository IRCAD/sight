/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/ui/qt/viz/PointEditor.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <geometry/data/IntrasecTypes.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

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

static const core::com::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

PointEditor::PointEditor() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &PointEditor::getInteraction, this);
}

//------------------------------------------------------------------------------

PointEditor::~PointEditor() noexcept
{
}

//------------------------------------------------------------------------------

void PointEditor::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

    QHBoxLayout* h_layout = new QHBoxLayout();

    QLabel* staticText_x = new QLabel(tr("x:"));
    h_layout->addWidget(staticText_x, 0, Qt::AlignVCenter);

    m_textCtrl_x = new QLineEdit();
    m_textCtrl_x->setValidator(new QDoubleValidator(m_textCtrl_x));
    h_layout->addWidget(m_textCtrl_x, 1, Qt::AlignVCenter);

    QLabel* staticText_y = new QLabel(tr("y:"));
    h_layout->addWidget(staticText_y, 0, Qt::AlignVCenter);

    m_textCtrl_y = new QLineEdit();
    m_textCtrl_y->setValidator(new QDoubleValidator(m_textCtrl_y));
    h_layout->addWidget(m_textCtrl_y, 1, Qt::AlignVCenter);

    QLabel* staticText_z = new QLabel(tr("z:"));
    h_layout->addWidget(staticText_z, 0, Qt::AlignVCenter);

    m_textCtrl_z = new QLineEdit();
    m_textCtrl_z->setValidator(new QDoubleValidator(m_textCtrl_z));
    h_layout->addWidget(m_textCtrl_z, 1, Qt::AlignVCenter);

    qtContainer->setLayout(h_layout);
    this->updating();
}

//------------------------------------------------------------------------------

void PointEditor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void PointEditor::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void PointEditor::updating()
{
}

//------------------------------------------------------------------------------

void PointEditor::getInteraction(data::tools::PickingInfo info)
{
    if(info.m_eventId == data::tools::PickingInfo::Event::MOUSE_LEFT_DOWN)
    {
        m_textCtrl_x->setText(QString("%1").arg(info.m_worldPos[0], 0, 'f', 3));
        m_textCtrl_y->setText(QString("%1").arg(info.m_worldPos[1], 0, 'f', 3));
        m_textCtrl_z->setText(QString("%1").arg(info.m_worldPos[2], 0, 'f', 3));
    }
}

//------------------------------------------------------------------------------

void PointEditor::info(std::ostream& _sstream)
{
    _sstream << "Point Editor";
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
