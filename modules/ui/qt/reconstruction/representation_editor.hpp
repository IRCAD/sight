/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/tools/failed.hpp>

#include <data/material.hpp>
#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QAbstractButton;

namespace sight::module::ui::qt::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction representation (surface, point, edge, ...).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::reconstruction::representation_editor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b reconstruction [sight::data::reconstruction]: reconstruction that will be updated
 */
class representation_editor : public QObject,
                              public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(representation_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    representation_editor() noexcept;

    /// Destructor. Do nothing.
    ~representation_editor() noexcept override;

protected:

    /// Initialize the UI
    void starting() override;

    /// Clean the UI
    void stopping() override;

    /// Update the UI according to the reconstruction
    void updating() override;

    /// Do nothing.
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Notify the changes
    void notify_material();

protected Q_SLOTS:

    void on_change_representation(int _id);
    void on_change_shading(int _id);
    void on_show_normals(int _state);

private:

    void refresh_normals();
    void refresh_representation();
    void refresh_shading();

    QPointer<QButtonGroup> m_button_group;
    QPointer<QButtonGroup> m_button_group_shading;
    QPointer<QButtonGroup> m_normals_radio_box;

    data::material::sptr m_material;

    static constexpr std::string_view RECONSTRUCTION = "reconstruction";
    data::ptr<data::reconstruction, data::access::inout> m_rec {this, RECONSTRUCTION};
};

} // namespace sight::module::ui::qt::reconstruction
