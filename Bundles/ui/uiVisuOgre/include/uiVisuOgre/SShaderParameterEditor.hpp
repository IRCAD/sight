/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/String.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IService.hpp>

#include <QVBoxLayout>

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
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction whose paremeters should be edited.
 */
class UIVISUOGRE_CLASS_API SShaderParameterEditor : public ::fwGui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SShaderParameterEditor)(::fwGui::editor::IEditor) )

    typedef std::string EditorImplementationType;
    typedef std::string ObjectClassnameType;
    typedef std::string ObjectId;
    typedef std::map< ObjectClassnameType, EditorImplementationType > EditorMapType;

    /// Constructor.
    UIVISUOGRE_API SShaderParameterEditor() noexcept;

    /// Destructor. Destroy UI
    UIVISUOGRE_API virtual ~SShaderParameterEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

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
        ::fwGuiQt::container::QtContainer::sptr editorPanel;
        ::fwServices::IService::wptr service;
        ::fwCom::helper::SigSlotConnection connections;
    };

    ShaderEditorInfo m_editorInfo;

    /// Connection to the material
    ::fwCom::helper::SigSlotConnection m_connections;

    QVBoxLayout* m_sizer;
};

} // uiVisuOgre
