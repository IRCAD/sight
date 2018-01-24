/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SShaderParameterEditor.hpp"

#include <uiVisuOgre/helper/ParameterEditor.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGui/GuiRegistry.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <visuOgreAdaptor/SMaterial.hpp>
#include <visuOgreAdaptor/SShaderParameter.hpp>

#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SShaderParameterEditor, ::fwData::Reconstruction);

//------------------------------------------------------------------------------
SShaderParameterEditor::SShaderParameterEditor() noexcept
{
}

//------------------------------------------------------------------------------

SShaderParameterEditor::~SShaderParameterEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::starting()
{
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections.connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->create();

    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    m_sizer = new QVBoxLayout();
    m_sizer->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(m_sizer);

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::stopping()
{
    m_connections.disconnect();
    this->clear();
    this->destroy();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::swapping()
{
    m_connections.disconnect();
    ::fwData::Reconstruction::sptr rec = this->getObject< ::fwData::Reconstruction >();
    ::fwData::Material::sptr material  = rec->getMaterial();
    m_connections.connect(material, this->getSptr(), this->getSptr()->getObjSrvConnections() );

    this->updating();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::updating()
{
    this->clear();
    this->updateGuiInfo();
    this->fillGui();
}

//------------------------------------------------------------------------------
void SShaderParameterEditor::clear()
{
    m_editorInfo.connections.disconnect();

    ::fwServices::IService::sptr objService = m_editorInfo.service.lock();

    if(objService)
    {
        objService->stop();

        ::fwGui::GuiRegistry::unregisterSIDContainer(m_editorInfo.uuid);

        ::fwServices::OSR::unregisterService(objService);

        m_sizer->removeWidget(m_editorInfo.editorPanel->getQtContainer());
        m_editorInfo.editorPanel->destroyContainer();
        m_editorInfo.editorPanel.reset();
    }
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

    bool found = false;

    // Is there at least one parameter that we can handle ?
    for (const auto& wParamSrv : matService->getRegisteredServices())
    {
        const auto paramSrv = wParamSrv.lock();
        if (paramSrv->getClassname() == "::visuOgreAdaptor::SShaderParameter")
        {
            /// Getting associated object infos
            const ::fwData::Object::csptr shaderObj = paramSrv->getObject();
            const ObjectClassnameType objType       = shaderObj->getClassname();

            if(objType == "::fwData::Boolean" || objType == "::fwData::Double" || objType == "::fwData::Integer")
            {
                found = true;
                break;
            }
        }
    }

    if(!found)
    {
        return;
    }

    /// Getting this widget's container
    auto qtContainer   = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* container = qtContainer->getQtContainer();

    QWidget* p2 = new QWidget( container );
    m_editorInfo.editorPanel = ::fwGuiQt::container::QtContainer::New();
    m_editorInfo.editorPanel->setQtContainer(p2);

    const std::string uuid = this->getID();
    m_editorInfo.uuid = uuid + "-editor";

    ::fwGui::GuiRegistry::registerSIDContainer(m_editorInfo.uuid, m_editorInfo.editorPanel);

    auto editorService = ::fwServices::add(
        this->getObject(), "::fwGui::editor::IEditor", "::guiQt::editor::SParameters",
        m_editorInfo.uuid );
    m_editorInfo.service = editorService;

    ::fwServices::IService::ConfigType editorConfig;

    // Get all ShaderParameter subservices from the corresponding Material adaptor
    for (auto wAdaptor : matService->getRegisteredServices())
    {
        const auto adaptor = wAdaptor.lock();
        if (adaptor->getClassname() == "::visuOgreAdaptor::SShaderParameter")
        {
            auto paramAdaptor = ::visuOgreAdaptor::SShaderParameter::dynamicCast(adaptor);
            auto paramConfig  = ::uiVisuOgre::helper::ParameterEditor::createConfig(paramAdaptor,
                                                                                    m_editorInfo.service.lock(),
                                                                                    m_editorInfo.connections);

            if(!paramConfig.empty())
            {
                editorConfig.add_child("parameters.param", paramConfig);
            }
        }
    }

    editorService->setConfiguration(editorConfig);
    editorService->configure();

    editorService->start();
}

//------------------------------------------------------------------------------

void SShaderParameterEditor::fillGui()
{
    auto editorService = m_editorInfo.service.lock();
    if(editorService)
    {
        m_sizer->addWidget( m_editorInfo.editorPanel->getQtContainer(), 0 );
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
