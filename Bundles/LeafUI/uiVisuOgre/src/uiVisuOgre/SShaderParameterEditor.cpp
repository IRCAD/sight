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

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <visuOgreAdaptor/SMaterial.hpp>
#include <visuOgreAdaptor/SShaderParameter.hpp>

#include <uiVisuOgre/helper/ParameterEditor.hpp>

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
    for (auto subSrv : matService->getRegisteredAdaptors())
    {
        if (subSrv.lock()->getClassname() == "::visuOgreAdaptor::SShaderParameter")
        {
            auto paramSrv = ::visuOgreAdaptor::SShaderParameter::dynamicCast(subSrv.lock());

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

    auto editorService = ::fwServices::add( this->getObject(), "::gui::editor::IEditor", "::guiQt::editor::SParameters",
                                            m_editorInfo.uuid );
    m_editorInfo.service = editorService;

    ::fwServices::IService::ConfigType editorConfig;

    // Get all ShaderParameter subservices from the corresponding Material adaptor
    for (auto adaptor : matService->getRegisteredAdaptors())
    {
        if (adaptor.lock()->getClassname() == "::visuOgreAdaptor::SShaderParameter")
        {
            auto paramAdaptor = ::visuOgreAdaptor::SShaderParameter::dynamicCast(adaptor.lock());
            auto paramConfig  = ::uiVisuOgre::helper::ParameterEditor::createConfig(paramAdaptor,
                                                                                    m_editorInfo.service.lock(),
                                                                                    m_editorInfo.connections);

            if(!paramConfig.empty())
            {
                editorConfig.add_child("service.parameters.param", paramConfig);
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
