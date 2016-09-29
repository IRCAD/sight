/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SCompositorParameterEditor.hpp"

#include <fwGui/GuiRegistry.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <visuOgreAdaptor/SCompositorParameter.hpp>

#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SCompositorParameterEditor, ::fwData::Object);

//------------------------------------------------------------------------------
SCompositorParameterEditor::SCompositorParameterEditor() throw()
{
}

//------------------------------------------------------------------------------

SCompositorParameterEditor::~SCompositorParameterEditor() throw()
{
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::configuring() throw(::fwTools::Failed)
{
    this->initialize();

    auto config = this->getConfigTree();

    m_renderID = config.get<std::string>("service.render.<xmlattr>.uid", "");
    m_layerID  = config.get<std::string>("service.layer.<xmlattr>.id", "");
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::starting() throw(::fwTools::Failed)
{
    this->create();

    auto qtContainer         = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);
    m_sizer = new QVBoxLayout(container);
    m_sizer->setContentsMargins(0,0,0,0);

    container->setLayout(m_sizer);

    m_render.reset();

    if(!m_renderID.empty())
    {
        auto services = ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

        for(const auto& renderer : services)
        {
            ::fwRenderOgre::SRender::sptr render = ::fwRenderOgre::SRender::dynamicCast(renderer);
            if(render->getName() == m_renderID)
            {
                m_render = render;

                if(!m_layerID.empty())
                {
                    const auto& layerMap = render->getLayers();
                    for(auto& layer : layerMap)
                    {
                        if(layer.first == m_layerID)
                        {
                            m_currentLayer = layer.second;

                            m_layerConnection.connect(m_currentLayer.lock(),
                                                      ::fwRenderOgre::Layer::s_COMPOSITOR_UPDATED_SIG,
                                                      this->getSptr(), s_UPDATE_SLOT);
                            break;
                        }
                    }
                }
            }
        }
    }

    SLM_ERROR_IF("SRender service '" + m_renderID + "' is not found.", m_render.expired());


    this->updating();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::stopping() throw(::fwTools::Failed)
{
    m_layerConnection.disconnect();
    this->clear();

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::updating() throw(::fwTools::Failed)
{
    this->clear();
    this->updateGuiInfo();
    this->fillGui();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::clear()
{
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

void SCompositorParameterEditor::updateGuiInfo()
{
    if(m_render.expired() || !m_currentLayer.lock())
    {
        return;
    }

    bool found = false;

    auto adaptors = m_currentLayer.lock()->getRegisteredAdaptors();

    // Is there at least one parameter that we can handle ?
    for (const auto& wAdaptor : adaptors)
    {
        auto adaptor = wAdaptor.lock();
        if (adaptor->getClassname() == "::visuOgreAdaptor::SCompositorParameter")
        {
            auto paramAdaptor = ::visuOgreAdaptor::SCompositorParameter::dynamicConstCast(adaptor);

            /// Getting associated object infos
            const ::fwData::Object::csptr shaderObj = ::fwServices::IService::constCast(paramAdaptor)->getObject();
            //paramAdaptor->getInputs().begin()->second.lock();
            const auto& objType = shaderObj->getClassname();

            if(objType == "::fwData::Boolean" || objType == "::fwData::Float" || objType == "::fwData::Integer")
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
    for (const auto& wAdaptor : adaptors)
    {
        auto adaptor = wAdaptor.lock();
        if (adaptor->getClassname() == "::visuOgreAdaptor::SCompositorParameter")
        {
            auto paramAdaptor = ::visuOgreAdaptor::SCompositorParameter::dynamicConstCast(adaptor);

            /// Getting associated object infos
            const ::fwData::Object::csptr shaderObj = ::fwServices::IService::constCast(paramAdaptor)->getObject();
            //paramAdaptor->getInputs().begin()->second.lock();
            const auto& objType = shaderObj->getClassname();

            if(objType == "::fwData::Boolean")
            {
                m_editorInfo.connections.connect(m_editorInfo.service.lock(), "boolChanged", paramAdaptor,
                                                 "setBoolParameter");

                ::fwServices::IService::ConfigType paramConfig;
                paramConfig.add("<xmlattr>.type", "bool");
                paramConfig.add("<xmlattr>.name", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.key", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", false);

                editorConfig.add_child("service.parameters.param", paramConfig);
            }
            else if(objType == "::fwData::Color")
            {
                m_editorInfo.connections.connect(m_editorInfo.service.lock(), "colorChanged", paramAdaptor,
                                                 "setColorParameter");

                ::fwServices::IService::ConfigType paramConfig;
                paramConfig.add("<xmlattr>.type", "color");
                paramConfig.add("<xmlattr>.name", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.key", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", "#ffffffff");

                editorConfig.add_child("service.parameters.param", paramConfig);
            }
            else if(objType == "::fwData::Float")
            {
                m_editorInfo.connections.connect(m_editorInfo.service.lock(), "doubleChanged", paramAdaptor,
                                                 "setDoubleParameter");

                const std::string& defaultValueStr = paramAdaptor->getDefaultValue();

                const double defaultValue = std::stod(defaultValueStr);
                const double max          = (defaultValue != 0.) ? defaultValue * 20. : 1.;
                const double min          = (defaultValue != 0.) ? max - defaultValue * 20. : 1.;

                ::fwServices::IService::ConfigType paramConfig;
                paramConfig.add("<xmlattr>.type", "double");
                paramConfig.add("<xmlattr>.name", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.key", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", defaultValueStr);
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);

                editorConfig.add_child("service.parameters.param", paramConfig);
            }
            else if(objType == "::fwData::Integer")
            {
                m_editorInfo.connections.connect(m_editorInfo.service.lock(), "intChanged", paramAdaptor,
                                                 "setIntParameter");

                const std::string& defaultValueStr = paramAdaptor->getDefaultValue();

                const int defaultValue = std::stoi(defaultValueStr);
                const int max          = (defaultValue != 0) ? defaultValue * 20 : 1;
                const int min          = (defaultValue != 0) ? max - defaultValue * 20 : 1;

                ::fwServices::IService::ConfigType paramConfig;
                paramConfig.add("<xmlattr>.type", "int");
                paramConfig.add("<xmlattr>.name", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.key", paramAdaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", defaultValueStr);
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);

                editorConfig.add_child("service.parameters.param", paramConfig);
            }
            else
            {
                OSLM_ERROR("No editor found for the object of type " << objType);
            }
        }
    }

    editorService->setConfiguration(editorConfig);
    editorService->configure();

    editorService->start();
}

//------------------------------------------------------------------------------

void SCompositorParameterEditor::fillGui()
{
    auto editorService = m_editorInfo.service.lock();
    if(editorService)
    {
        m_sizer->addWidget( m_editorInfo.editorPanel->getQtContainer(), 0 );
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
