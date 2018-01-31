/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuOgre/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <QVBoxLayout>

namespace uiVisuOgre
{

/**
 * @brief   Editor allowing to edit shader uniform from a compositor material.

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::uiVisuOgre::SCompositorParameterEditor" >

       </service>
       @endcode
 */
class UIVISUOGRE_CLASS_API SCompositorParameterEditor : public ::fwGui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SCompositorParameterEditor)(::fwGui::editor::IEditor) )

    /// Constructor.
    UIVISUOGRE_API SCompositorParameterEditor() noexcept;

    /// Destructor. Destroy UI
    UIVISUOGRE_API virtual ~SCompositorParameterEditor() noexcept;

protected:

    /// Configure the editor to associate with each object type
    UIVISUOGRE_API virtual void configuring() override;

    /// Start the service, slot connections, QtContainer initialization .
    UIVISUOGRE_API virtual void starting() override;

    /// Destroy the service and the container.
    UIVISUOGRE_API virtual void stopping() override;

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

    QVBoxLayout* m_sizer;

    /// Name of the render service when we configure it in XML
    std::string m_renderID;

    /// Name of the layer when we configure it in XML
    std::string m_layerID;

    /// Pointer on the render service we work on
    ::fwRenderOgre::SRender::cwptr m_render;

    /// Pointer on the layer we work on
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    /// Connection with the layer to be aware of compositors updates
    ::fwCom::helper::SigSlotConnection m_layerConnection;
};

} // uiVisuOgre
