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

#include <core/com/helper/sig_slot_connection.hpp>

#include <data/Reconstruction.hpp>
#include <data/String.hpp>

#include <service/base.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::viz
{

/**
 * @brief   Editor allowing to edit each parameters from each shader of a reconstruction
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::viz::SShaderParameterEditor" >
            <inout key="reconstruction" uid="..." />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction whose paremeters should be edited.
 */
class MODULE_UI_VIZ_CLASS_API SShaderParameterEditor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(SShaderParameterEditor, sight::ui::editor);

    typedef std::string EditorImplementationType;
    typedef std::string ObjectClassnameType;
    typedef std::string ObjectId;
    typedef std::map<ObjectClassnameType, EditorImplementationType> EditorMapType;

    /// Constructor.
    MODULE_UI_VIZ_API SShaderParameterEditor() noexcept;

    /// Destructor. Destroy UI
    MODULE_UI_VIZ_API ~SShaderParameterEditor() noexcept override;

protected:

    /// Initialize the container
    MODULE_UI_VIZ_API void configuring() override;

    /// Start the service, slot connections, widget initialization .
    MODULE_UI_VIZ_API void starting() override;

    /// Destroy the service and the container.
    MODULE_UI_VIZ_API void stopping() override;

    /// Update the interface.
    MODULE_UI_VIZ_API void updating() override;

private:

    /// Clear the current container
    void clear();
    /// Retrieves the shader parameters attached to the Reconstruction object and stores them into a collection
    void updateGuiInfo();
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

    /// Connection to the material
    core::com::helper::sig_slot_connection m_connections;

    QVBoxLayout* m_sizer {};

    data::ptr<data::Reconstruction, data::Access::inout> m_reconstruction {this, "reconstruction"};
};

} // namespace sight::module::ui::viz
