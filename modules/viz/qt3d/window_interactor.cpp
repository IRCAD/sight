/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "window_interactor.hpp"

#include <ui/qt/container/widget.hpp>

#include <viz/qt3d/registry/macros.hpp>
#include <viz/qt3d/render.hpp>

#include <Qt3DExtras/Qt3DWindow>

#include <QVBoxLayout>
#include <QWidget>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_RENDER_QT3D(
    sight::module::viz::qt3d::window_interactor,
    sight::viz::qt3d::window_interactor::REGISTRY_KEY
);

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d
{

//-----------------------------------------------------------------------------

void window_interactor::create_container(ui::container::widget::sptr _parent)
{
    SIGHT_ASSERT("Invalid parent.", _parent);
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);

    auto* view_3d = new Qt3DExtras::Qt3DWindow();
    this->set_3d_view(view_3d);
}

//-----------------------------------------------------------------------------

void window_interactor::set_3d_view(Qt3DExtras::Qt3DWindow* _3d_view)
{
    m_parent_container->clean();

    m_window_container = QWidget::createWindowContainer(_3d_view);
    QPointer<QVBoxLayout> layout = new QVBoxLayout;
    layout->addWidget(m_window_container);
    layout->setContentsMargins(0, 0, 0, 0);
    m_parent_container->set_layout(layout);
}

} // namespace sight::module::viz::qt3d
