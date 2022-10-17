/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "WindowInteractor.hpp"

#include <ui/qt/container/QtContainer.hpp>

#include <viz/qt3d/registry/macros.hpp>
#include <viz/qt3d/SRender.hpp>

#include <Qt3DExtras/Qt3DWindow>

#include <QVBoxLayout>
#include <QWidget>

//-----------------------------------------------------------------------------

SIGHT_REGISTER_RENDER_QT3D(
    sight::module::viz::qt3d::WindowInteractor,
    sight::viz::qt3d::IWindowInteractor::REGISTRY_KEY
);

//-----------------------------------------------------------------------------

namespace sight::module::viz::qt3d
{

//-----------------------------------------------------------------------------

WindowInteractor::WindowInteractor(sight::viz::qt3d::IWindowInteractor::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

WindowInteractor::~WindowInteractor()
= default;

//-----------------------------------------------------------------------------

void WindowInteractor::createContainer(ui::base::container::fwContainer::sptr _parent)
{
    SIGHT_ASSERT("Invalid parent.", _parent);
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(_parent);

    auto* view3D = new Qt3DExtras::Qt3DWindow();
    this->set3DView(view3D);
}

//-----------------------------------------------------------------------------

void WindowInteractor::set3DView(Qt3DExtras::Qt3DWindow* _3dView)
{
    m_parentContainer->clean();

    m_windowContainer = QWidget::createWindowContainer(_3dView);
    QPointer<QVBoxLayout> layout = new QVBoxLayout;
    layout->addWidget(m_windowContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    m_parentContainer->setLayout(layout);
}

} // namespace sight::module::viz::qt3d
