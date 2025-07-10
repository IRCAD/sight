/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QPushButton;
class QLabel;

namespace sight::module::ui::qt::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction material (color and transparency).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::reconstruction::organ_material_editor">
       <inout key="reconstruction" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b reconstruction [sight::data::reconstruction]: reconstruction containing the material to update.
 */
class organ_material_editor : public QObject,
                              public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(organ_material_editor, sight::ui::editor);

    /// Creates the service.
    organ_material_editor() noexcept;

    /// Destroys the service.
    ~organ_material_editor() noexcept override;

private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::object::MODIFIED_SIG of s_RECONSTRUCTION_INOUT to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Configures the service.
    void configuring() final;

    /// Sets the connections and the UI elements.
    void starting() final;

    /// Updates the UI according to the material (color and transparency widgets).
    void updating() final;

    /// Destroys the connections and cleans the container.
    void stopping() final;

    /// Updates the UI according to the material (color and transparency widgets)
    void refresh_material();

    /// Notifies the material changes.
    void material_notification();

    QPointer<QPushButton> m_diffuse_colour_button;
    QPointer<QPushButton> m_ambient_colour_button;
    QPointer<QSlider> m_opacity_slider;
    QPointer<QLabel> m_transparency_value;

    static constexpr std::string_view RECONSTRUCTION = "reconstruction";
    data::ptr<data::reconstruction, data::access::inout> m_rec {this, RECONSTRUCTION};

private Q_SLOTS:

    /**
     * @brief Slot: called when the opacity slider changed.
     * @param _value The new opacity value.
     */
    void on_opacity_slider(int _value);

    /// Slot: called when the diffuse color button is clicked.
    void on_diffuse_color_button();

    /// Slot: called when the ambient color button is clicked.
    void on_ambient_color_button();
};

} // namespace sight::module::ui::qt::reconstruction
