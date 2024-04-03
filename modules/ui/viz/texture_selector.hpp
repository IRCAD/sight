/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
class texture_selector : public QObject,
                         public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(texture_selector, sight::ui::editor);

    texture_selector() noexcept;
    ~texture_selector() noexcept override;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="texture_selectorInstance" type="sight::module::ui::viz::texture_selector">
        </service>
       @endcode
     */
    void configuring() override;

    /// Start the service. Create UI
    void starting() override;

    /// Stop the service. Delete UI
    void stopping() override;

    /// Do nothing
    void updating() override;

protected Q_SLOTS:

    void on_load_button();
    void on_delete_button();

private:

    QPointer<QPushButton> m_load_button;
    QPointer<QPushButton> m_delete_button;

    data::ptr<data::reconstruction, data::access::inout> m_reconstruction {this, "reconstruction"};
};

} // namespace sight::module::ui::viz
