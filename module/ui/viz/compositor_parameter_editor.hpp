/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <ui/__/editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/layer.hpp>
#include <viz/scene3d/render.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::viz
{

/**
 * @brief   Editor allowing to edit shader uniform from a compositor material.

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::viz::compositor_parameter_editor" >

       </service>
       @endcode
 */
class compositor_parameter_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(compositor_parameter_editor, sight::ui::editor);

    /// Constructor.
    compositor_parameter_editor() noexcept;

    /// Destructor. Destroy UI
    ~compositor_parameter_editor() noexcept override;

    /**
     * @name Slots API
     * @{
     */
    /// Slot: update the interface.
    static const core::com::slots::key_t UPDATE_COMPOSITOR_SLOT;
/** @} */

protected:

    /// Configure the editor to associate with each object type
    void configuring() override;

    /// Start the service, slot connections, widget initialization .
    void starting() override;

    /// Destroy the service and the container.
    void stopping() override;

    /// Update the interface.
    void updating() override;

private:

    /// Slot: update the interface.
    void update_compositor(std::string _compositor_name, bool _enabled, sight::viz::scene3d::layer::sptr _layer);

    /// Clear the current container
    void clear();
    /// Instanciates the needed ui editors according to the stored informations
    void fill_gui();

    /// Internal class that contain the informations concerning the editor that is created.
    struct shader_editor_info
    {
        std::string uuid;
        sight::ui::qt::container::widget::sptr editor_panel;
        service::base::wptr srv;
    };

    shader_editor_info m_editor_info;

    QVBoxLayout* m_sizer {};

    /// Name of the layer when we configure it in XML
    std::string m_layer_id;
};

} // namespace sight::module::ui::viz
