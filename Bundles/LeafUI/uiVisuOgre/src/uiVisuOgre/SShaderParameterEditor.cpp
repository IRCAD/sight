/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SShaderParameterEditor.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGui/GuiRegistry.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <visuOgreAdaptor/SMaterial.hpp>
#include <visuOgreAdaptor/SShaderParameter.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SShaderParameterEditor, ::fwData::Reconstruction);

//------------------------------------------------------------------------------
SShaderParameterEditor::SShaderParameterEditor() throw()
{
}

//------------------------------------------------------------------------------

SShaderParameterEditor::~SShaderParameterEditor() throw()
{
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::starting() throw(::fwTools::Failed)
{
    m_connections                      = ::fwServices::helper::SigSlotConnection::New();
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections->connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);
    m_gridSizer = new QGridLayout(container);
    m_gridSizer->setContentsMargins(0,0,0,0);

    container->setLayout(m_gridSizer);

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::stopping() throw(::fwTools::Failed)
{
    this->clear();

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::swapping() throw(::fwTools::Failed)
{
    m_connections->disconnect();
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections->connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::configuring() throw(::fwTools::Failed)
{
    this->initialize();

    SLM_ASSERT("Not a service configuration", m_configuration->getName() == "service");

    OSLM_WARN_IF(
        "missing associations for " << this->getID() << "configuration",
        !m_configuration->findConfigurationElement("association") );

    std::vector < Configuration > vectConfig = m_configuration->find("association");

    for(auto element : vectConfig)
    {
        OSLM_FATAL_IF( "missing 'type' attribute for " << this->getID() << "configuration",
                       !element->hasAttribute("type") );
        OSLM_FATAL_IF( "missing 'editor' attribute for " << this->getID() << "configuration",
                       !element->hasAttribute("editor") );

        m_associatedEditor[element->getAttributeValue("type")] = element->getAttributeValue("editor");
    }
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updating() throw(::fwTools::Failed)
{
    this->clear();
    this->updateGuiInfo();
    this->fillGui();
}

//------------------------------------------------------------------------------
void SShaderParameterEditor::clear()
{
    for(auto info : m_shaderEditorInfos)
    {
        info.connections->disconnect();

        ::fwServices::IService::sptr objService   = info.objService.lock();
        ::fwServices::IService::sptr labelService = info.labelService.lock();

        objService->stop();
        labelService->stop();

        ::fwGui::GuiRegistry::unregisterSIDContainer(info.labelUUID);
        ::fwGui::GuiRegistry::unregisterSIDContainer(info.editorUUID);

        ::fwServices::OSR::unregisterService(objService);
        ::fwServices::OSR::unregisterService(labelService);

        m_gridSizer->removeWidget(info.editorPanel->getQtContainer());
        info.editorPanel->destroyContainer();
        info.editorPanel.reset();

        m_gridSizer->removeWidget(info.labelPanel->getQtContainer());
        info.labelPanel->destroyContainer();
        info.labelPanel.reset();
        info.label.reset();
    }

    m_shaderEditorInfos.clear();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updateGuiInfo()
{
    /// Getting all Material adaptors
    ::fwData::Reconstruction::sptr reconstruction                   = this->getObject< ::fwData::Reconstruction >();
    ::fwServices::registry::ObjectService::ServiceVectorType srvVec = ::fwServices::OSR::getServices(
        "::visuOgreAdaptor::SMaterial");

    /// Stop if no Material adaptors have been find
    if(srvVec.empty())
    {
        SLM_WARN("No ::visuOgreAdaptor::SMaterial found in the application");
        return;
    }

    /// Try to find the material adaptor working with the same ::fwData::Material
    /// as the one contained by the current reconstruction
    ::fwRenderOgre::IAdaptor::sptr matService;
    for (auto srv : srvVec)
    {
        if (srv->getObject()->getID() == reconstruction->getMaterial()->getID())
        {
            matService = ::fwRenderOgre::IAdaptor::dynamicCast(srv);
            break;
        }
    }

    SLM_ASSERT("Material adaptor corresponding to the current Reconstruction must exist", matService);

    /// Get all ShaderParameter subservices from the corresponding Material adaptor
    for (auto subSrv : matService->getRegisteredAdaptors())
    {
        if (subSrv.lock()->getClassname() == "::visuOgreAdaptor::SShaderParameter")
        {
            ::visuOgreAdaptor::SShaderParameter::sptr shaderSrv =
                ::visuOgreAdaptor::SShaderParameter::dynamicCast(subSrv.lock());

            /// Getting this widget's container
            ::fwGuiQt::container::QtContainer::sptr qtContainer =
                ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
            QWidget* container = qtContainer->getQtContainer();

            /// Getting associated object infos
            ::fwData::Object::sptr shaderObj = shaderSrv->getObject();
            ObjectClassnameType objType = shaderObj->getClassname();
            ObjectId key                = shaderObj->getID();
            ObjectId name               = shaderObj->getName();

            std::string uuid = this->getID();
            std::string editor;

            if (m_associatedEditor.find(objType) != m_associatedEditor.end())
            {
                editor = m_associatedEditor[objType];
            }
            else
            {
                OSLM_ERROR("No editor found for the object of type " << objType);
            }

            /// Fill editors informations depending on data type
            if (!editor.empty())
            {
                ShaderEditorInfo info;

                info.labelPanel = ::fwGuiQt::container::QtContainer::New();
                QWidget * p1 = new QWidget( container);
                info.labelPanel->setQtContainer(p1);

                info.editorPanel = ::fwGuiQt::container::QtContainer::New();
                QWidget * p2 = new QWidget( container );
                info.editorPanel->setQtContainer(p2);

                info.labelUUID  = uuid + "-label-" + key;
                info.editorUUID = uuid + "-editee-" + key;

                ::fwGui::GuiRegistry::registerSIDContainer(info.labelUUID, info.labelPanel);
                ::fwGui::GuiRegistry::registerSIDContainer(info.editorUUID, info.editorPanel);

                info.label = ::fwData::String::New(name);

                ::fwServices::IService::sptr objService;
                ::fwServices::IService::sptr labelService;
                objService   = ::fwServices::add( shaderObj, "::gui::editor::IEditor", editor, info.editorUUID );
                labelService = ::fwServices::add( info.label, "::gui::editor::IEditor", "::uiData::SStaticTextEditor",
                                                  info.labelUUID );

                info.objService   = objService;
                info.labelService = labelService;

                objService->configure();
                labelService->configure();

                objService->start();
                labelService->start();

                info.connections->connect( shaderObj, objService, objService->getObjSrvConnections() );

                m_shaderEditorInfos.push_back(info);

                OSLM_TRACE("Created container " << info.editorUUID << " for " << shaderObj->getID() );
            }
        }
    }
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::fillGui()
{
    int line = 0;

    for(auto info : m_shaderEditorInfos)
    {
        m_gridSizer->addWidget( info.labelPanel->getQtContainer(), line, 0 );
        m_gridSizer->addWidget( info.editorPanel->getQtContainer(), line, 1 );
        line++;
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
