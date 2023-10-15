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

#include <ui/__/editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/Layer.hpp>
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
class MODULE_UI_VIZ_CLASS_API compositor_parameter_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(compositor_parameter_editor, sight::ui::editor);

    /// Constructor.
    MODULE_UI_VIZ_API compositor_parameter_editor() noexcept;

    /// Destructor. Destroy UI
    MODULE_UI_VIZ_API ~compositor_parameter_editor() noexcept override;

    /**
     * @name Slots API
     * @{
     */
    /// Slot: update the interface.
    MODULE_UI_VIZ_API static const core::com::slots::key_t UPDATE_COMPOSITOR_SLOT;
/** @} */

protected:

    /// Configure the editor to associate with each object type
    MODULE_UI_VIZ_API void configuring() override;

    /// Start the service, slot connections, widget initialization .
    MODULE_UI_VIZ_API void starting() override;

    /// Destroy the service and the container.
    MODULE_UI_VIZ_API void stopping() override;

    /// Update the interface.
    MODULE_UI_VIZ_API void updating() override;

private:

    /// Slot: update the interface.
    void updateCompositor(std::string _compositorName, bool _enabled, sight::viz::scene3d::Layer::sptr _layer);

    /// Clear the current container
    void clear();
    /// Instanciates the needed ui editors according to the stored informations
    void fillGui();

    /// Internal class that contain the informations concerning the editor that is created.
    struct ShaderEditorInfo
    {
        std::string uuid;
        sight::ui::qt::container::widget::sptr editorPanel;
        service::base::wptr srv;
        core::com::helper::sig_slot_connection connections;
    };

    ShaderEditorInfo m_editorInfo;

    QVBoxLayout* m_sizer {};

    /// Name of the layer when we configure it in XML
    std::string m_layerID;
};

} // namespace sight::module::ui::viz
