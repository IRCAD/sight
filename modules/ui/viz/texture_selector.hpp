/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select a data::image and apply it to the current reconstruction as an Ogre texture
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::viz::texture_selector">
            <inout key="reconstruction" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::reconstruction]: reconstruction where the texture should be applied.
 */
class MODULE_UI_VIZ_CLASS_API texture_selector : public QObject,
                                                 public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(texture_selector, sight::ui::editor);

    MODULE_UI_VIZ_API texture_selector() noexcept;
    MODULE_UI_VIZ_API ~texture_selector() noexcept override;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="texture_selectorInstance" type="sight::module::ui::viz::texture_selector">
        </service>
       @endcode
     */
    MODULE_UI_VIZ_API void configuring() override;

    /// Start the service. Create UI
    MODULE_UI_VIZ_API void starting() override;

    /// Stop the service. Delete UI
    MODULE_UI_VIZ_API void stopping() override;

    /// Do nothing
    MODULE_UI_VIZ_API void updating() override;

protected Q_SLOTS:

    void on_load_button();
    void on_delete_button();

private:

    QPointer<QPushButton> m_load_button;
    QPointer<QPushButton> m_delete_button;

    data::ptr<data::reconstruction, data::access::inout> m_reconstruction {this, "reconstruction"};
};

} // namespace sight::module::ui::viz
