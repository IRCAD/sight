/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SSHADERPARAMETEREDITOR_HPP__
#define __UIVISUOGRE_SSHADERPARAMETEREDITOR_HPP__

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/SigSlotConnection.hpp>
#include <fwServices/IService.hpp>


#include <gui/editor/IEditor.hpp>

#include <QGridLayout>

#include "uiVisuOgre/config.hpp"

namespace uiVisuOgre
{

/**
 * @brief   Editor allowing to edit each parameters from each shader of a reconstruction
 * @class   SShaderParameterEditor
 */
class UIVISUOGRE_CLASS_API SShaderParameterEditor : public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SShaderParameterEditor)(::gui::editor::IEditor) );

    typedef std::string EditorImplementationType;
    typedef std::string ObjectClassnameType;
    typedef std::string ObjectId;
    typedef std::map< ObjectClassnameType, EditorImplementationType > EditorMapType;

    /// Constructor.
    UIVISUOGRE_API SShaderParameterEditor() throw();

    /// Destructor. Destroy UI
    UIVISUOGRE_API virtual ~SShaderParameterEditor() throw();

protected:

    /// Internal class that will contain the Qt components foa one specific shader.
    class ShaderEditorInfo
    {

    public:

        ShaderEditorInfo()
        {
            connections = ::fwServices::helper::SigSlotConnection::New();
        }

        ::fwGuiQt::container::QtContainer::sptr labelPanel;
        ::fwGuiQt::container::QtContainer::sptr editorPanel;

        std::string labelUUID;
        std::string editorUUID;

        ::fwData::String::sptr label;

        ::fwServices::IService::wptr labelService;
        ::fwServices::IService::wptr objService;

        ::fwServices::helper::SigSlotConnection::sptr connections;
    };
    typedef std::vector< ShaderEditorInfo > ShaderEditorInfoContainerType;
    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Configure the editor to associate with each object type
     *
     * @verbatim
       <service uid="paramView" implementation="::uiVisuOgre::SShaderParameterEditor" type="::gui::editor::IEditor" autoComChannel="yes">
         <association type="::fwData::Float" editor="::uiData::SFloatEditor" />
         <association type="::fwData::Boolean" editor="::uiData::SBooleanEditor" />
         <association type="::fwData::Integer" editor="::uiData::SIntegerEditor" />
         <association type="::fwData::Image" editor="::uiData::SImageSceneEditorSrv" />
       </service>
       @endverbatim
     * for which:\n
     * - Each <association> node must contain a type attribute and an editor attribute.\n
     * It defines a rule for all the data which has the specify type in type attribute.
     * - type attribute is a fw4spl data type
     * - editor attribute is the editor associated to the type given in association.
     */
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Start the service, slot connections, QtContainer initialization .
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Destroy the service and the container.
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Called once we change the data.
    UIVISUOGRE_API virtual void swapping() throw ( ::fwTools::Failed );

    /// Update the interface.
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

    ShaderEditorInfoContainerType m_shaderEditorInfos;
    EditorMapType m_associatedEditor;

    ::fwServices::helper::SigSlotConnection::sptr m_connections; ///< Connection to image

private:

    /// Clear the current container
    void clear();
    /// Retrieves the shader parameters attached to the Reconstruction object and stores them into a collection
    void updateGuiInfo();
    /// Instanciates the needed ui editors according to the stored informations
    void fillGui();

    QGridLayout* m_gridSizer;

};

} // uiVisuOgre

#endif // __UIVISUOGRE_SSHADERPARAMETEREDITOR_HPP__
