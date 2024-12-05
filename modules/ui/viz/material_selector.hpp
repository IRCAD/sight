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

#include <core/tools/failed.hpp>

#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QString>

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select an Ogre material template and apply it to the current reconstruction.
 *
 * @section Signals Signals
 * - \b selected(std::string) : Send the selected material name.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::viz::material_selector">
            <inout key="reconstruction" uid="..." />
            <config materials="Basic,Default,OrenNayar,Foggy" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::object]: reconstruction used to select the material to change.
 */

/**
 * @brief
 */
class material_selector : public QObject,
                          public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(material_selector, sight::ui::editor);

    /// Constructor.
    material_selector() noexcept;

    /// Destructor. Does nothing
    ~material_selector() noexcept override = default;

    /**
     * @name Signals API
     * @{
     */
    static const core::com::signals::key_t SELECTED_SIG;
    using selected_signal_t = core::com::signal<void (std::string)>;
/** @} */

protected:

    /// Get the list of supported materials
    void configuring(const config_t& _config) override;

    /// Start the service. Create UI
    void starting() override;

    /// Stop the service. Delete UI
    void stopping() override;

    /// Update UI depending on current reconstruction
    void updating() override;

protected Q_SLOTS:

    /// SLOT: Called when a material is selected
    void on_selected_mode_item(const QString& _text);

    /// SLOT: Called when the reload button is pushed
    void on_reload_material();

private:

    void update_material();

    QPointer<QComboBox> m_material_box;
    QPointer<QPushButton> m_reload_button;

    /// Configurable list of material that will be exposed, if they are loaded
    std::set<std::string> m_supported_materials {"Default"};

    data::ptr<data::reconstruction, data::access::inout> m_reconstruction {this, "reconstruction"};
};

} // namespace sight::module::ui::viz
