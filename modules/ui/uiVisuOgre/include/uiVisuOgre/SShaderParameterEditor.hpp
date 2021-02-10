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

#include "uiVisuOgre/config.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/runtime/ConfigurationElement.hpp>

#include <data/String.hpp>

#include <services/IService.hpp>

#include <QVBoxLayout>

#include <ui/base/editor/IEditor.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace uiVisuOgre
{

/**
 * @brief   Editor allowing to edit each parameters from each shader of a reconstruction
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::uiVisuOgre::SShaderParameterEditor" >
            <inout key="reconstruction" uid="..." />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction whose paremeters should be edited.
 */
class UIVISUOGRE_CLASS_API SShaderParameterEditor : public sight::ui::base::editor::IEditor
{

public:

    fwCoreServiceMacro(SShaderParameterEditor, sight::ui::base::editor::IEditor)

    typedef std::string EditorImplementationType;
    typedef std::string ObjectClassnameType;
    typedef std::string ObjectId;
    typedef std::map< ObjectClassnameType, EditorImplementationType > EditorMapType;

    /// Constructor.
    UIVISUOGRE_API SShaderParameterEditor() noexcept;

    /// Destructor. Destroy UI
    UIVISUOGRE_API virtual ~SShaderParameterEditor() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Initialize the container
    UIVISUOGRE_API virtual void configuring() override;

    /// Start the service, slot connections, QtContainer initialization .
    UIVISUOGRE_API virtual void starting() override;

    /// Destroy the service and the container.
    UIVISUOGRE_API virtual void stopping() override;

    /// Called once we change the data.
    UIVISUOGRE_API virtual void swapping() override;

    /// Update the interface.
    UIVISUOGRE_API virtual void updating() override;

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
        ui::qt::container::QtContainer::sptr editorPanel;
        services::IService::wptr service;
        core::com::helper::SigSlotConnection connections;
    };

    ShaderEditorInfo m_editorInfo;

    /// Connection to the material
    core::com::helper::SigSlotConnection m_connections;

    QVBoxLayout* m_sizer;
};

} // uiVisuOgre
