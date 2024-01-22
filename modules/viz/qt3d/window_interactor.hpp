/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#pragma once

#include "modules/viz/qt3d/config.hpp"

#include <core/base.hpp>

#include <ui/__/container/widget.hpp>

#include <viz/qt3d/window_interactor.hpp>

#include <QObject>
#include <QPointer>

namespace sight::ui::qt::container
{

class widget;

} // namespace sight::ui::qt::container

namespace Qt3DExtras
{

class Qt3DWindow;

} // namespace Qt3DExtras

namespace sight::module::viz::qt3d
{

/// Defines a class to manage a render window.
class MODULE_VIZ_QT3D_CLASS_API window_interactor final : public QObject,
                                                          public sight::viz::qt3d::window_interactor
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(window_interactor, sight::viz::qt3d::window_interactor);

    /// Destroys the window container.
    MODULE_VIZ_QT3D_API ~window_interactor() final = default;
    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     */
    MODULE_VIZ_QT3D_API void create_container(ui::container::widget::sptr _parent) final;

    MODULE_VIZ_QT3D_API void set_3d_view(Qt3DExtras::Qt3DWindow* _3d_view) final;

private:

    /// Contains the parent of the widget.
    SPTR(ui::qt::container::widget) m_parent_container;

    /// Contains the window container.
    QPointer<QWidget> m_window_container;
};

} // namespace sight::module::viz::qt3d.
