/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <ui/base/container/fwContainer.hpp>

#include <viz/qt3d/IWindowInteractor.hpp>

#include <QObject>
#include <QPointer>

namespace sight::ui::qt
{

namespace container
{

class QtContainer;

}

}

namespace Qt3DExtras
{

class Qt3DWindow;

}

namespace sight::module::viz::qt3d
{

/// Defines a class to manage a render window.
class MODULE_VIZ_QT3D_CLASS_API WindowInteractor final : public QObject,
                                                         public sight::viz::qt3d::IWindowInteractor
{
Q_OBJECT

public:

    SIGHT_DECLARE_CLASS(WindowInteractor, sight::viz::qt3d::IWindowInteractor);

    /// Initialize members.
    MODULE_VIZ_QT3D_API WindowInteractor(sight::viz::qt3d::IWindowInteractor::Key);

    /// Destroys the window container.
    MODULE_VIZ_QT3D_API ~WindowInteractor() final;
    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     * @param _renderOnDemand if true, the rendering will be done only when it's requested.
     * @param _fullscreen enable the fullscreen.
     */
    MODULE_VIZ_QT3D_API void createContainer(ui::base::container::fwContainer::sptr _parent) final;

    MODULE_VIZ_QT3D_API void set3DView(Qt3DExtras::Qt3DWindow* _3dView) final;

private:

    /// Contains the parent of the widget.
    SPTR(ui::qt::container::QtContainer) m_parentContainer;

    /// Contains the window container.
    QPointer<QWidget> m_windowContainer;
};

} // namespace sight::module::viz::qt3d.
