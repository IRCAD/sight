/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <viz/scene3d/Layer.hpp>
#include <viz/scene3d/SRender.hpp>

#include <QVBoxLayout>

#include <ui/base/IEditor.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::modules::ui::viz
{

/**
 * @brief   Editor allowing to edit shader uniform from a compositor material.

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::sight::modules::ui::viz::SCompositorParameterEditor" >

       </service>
       @endcode
 */
class MODULE_UI_VIZ_CLASS_API SCompositorParameterEditor : public sight::ui::base::IEditor
{

public:

    fwCoreServiceMacro(SCompositorParameterEditor, sight::ui::base::IEditor)

    /// Constructor.
    MODULE_UI_VIZ_API SCompositorParameterEditor() noexcept;

    /// Destructor. Destroy UI
    MODULE_UI_VIZ_API virtual ~SCompositorParameterEditor() noexcept;

    /**
     * @name Slots API
     * @{
     */
    /// Slot: update the interface.
    MODULE_UI_VIZ_API static const core::com::Slots::SlotKeyType s_UPDATE_COMPOSITOR_SLOT;
    /** @} */

protected:

    /// Configure the editor to associate with each object type
    MODULE_UI_VIZ_API virtual void configuring() override;

    /// Start the service, slot connections, QtContainer initialization .
    MODULE_UI_VIZ_API virtual void starting() override;

    /// Destroy the service and the container.
    MODULE_UI_VIZ_API virtual void stopping() override;

    /// Update the interface.
    MODULE_UI_VIZ_API virtual void updating() override;

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
        sight::ui::qt::container::QtContainer::sptr editorPanel;
        service::IService::wptr service;
        core::com::helper::SigSlotConnection connections;
    };

    ShaderEditorInfo m_editorInfo;

    QVBoxLayout* m_sizer;

    /// Name of the layer when we configure it in XML
    std::string m_layerID;
};

} // uiVisuOgre
