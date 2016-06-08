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
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections.connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->create();

    auto qtContainer         = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);
    m_sizer = new QVBoxLayout(container);
    m_sizer->setContentsMargins(0,0,0,0);

    container->setLayout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::stopping() throw(::fwTools::Failed)
{
    m_connections.disconnect();
    this->clear();

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::swapping() throw(::fwTools::Failed)
{
    m_connections.disconnect();
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections.connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::configuring() throw(::fwTools::Failed)
{
    this->initialize();
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

        ::fwServices::IService::sptr objService = info.objService.lock();

        objService->stop();

        ::fwGui::GuiRegistry::unregisterSIDContainer(info.editorUUID);

        ::fwServices::OSR::unregisterService(objService);

        m_sizer->removeWidget(info.editorPanel->getQtContainer());
        info.editorPanel->destroyContainer();
        info.editorPanel.reset();
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

            if(objType == "::fwData::Integer")
            {
                editor = "::guiQt::editor::SSlider";
            }
            else
            {
                OSLM_ERROR("No editor found for the object of type " << objType);
            }

            /// Fill editors informations depending on data type
            if (!editor.empty())
            {
                ShaderEditorInfo info;

                QWidget * p2 = new QWidget( container );
                info.editorPanel = ::fwGuiQt::container::QtContainer::New();
                info.editorPanel->setQtContainer(p2);

                info.editorUUID = uuid + "-editee-" + key;

                ::fwGui::GuiRegistry::registerSIDContainer(info.editorUUID, info.editorPanel);

                ::fwServices::IService::sptr objService;
                objService = ::fwServices::add( shaderObj, "::gui::editor::IEditor", editor, info.editorUUID );

                info.objService = objService;

                ::fwServices::IService::ConfigType config;
                config.add("config.range.min", 0);
                config.add("config.range.max", 100);
                config.add("config.defaultValue", 10);
                config.add("config.value", 10);
                config.add("config.text", name);

                objService->setConfiguration(config);
                objService->configure();

                objService->start();

                info.connections->connect(objService, "valueChanged", shaderSrv, "setIntParameter");

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
        m_sizer->addWidget( info.editorPanel->getQtContainer(), line );
        line++;
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
