#include "uiVisuOgre/SLightSelector.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/ILight.hpp>
#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SLightSelector, ::fwData::Composite );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLightSelector::s_INIT_LIGHT_LIST_SLOT = "initLightList";

//------------------------------------------------------------------------------

SLightSelector::SLightSelector() throw()
{
    newSlot(s_INIT_LIGHT_LIST_SLOT, &SLightSelector::initLightList, this);
}

//------------------------------------------------------------------------------

SLightSelector::~SLightSelector() throw()
{
}

//------------------------------------------------------------------------------

void SLightSelector::starting() throw(::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", container);

    m_layersBox  = new QComboBox(container);
    m_lightsList = new QListWidget(container);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_lightsList);

    container->setLayout(layout);

    this->refreshLayers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
}

//------------------------------------------------------------------------------

void SLightSelector::stopping() throw(::fwTools::Failed)
{
    m_connections.disconnect();

    QObject::disconnect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SLightSelector::configuring() throw(::fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SLightSelector::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SLightSelector::onSelectedLayerItem(int index)
{
    m_currentLayer = m_layers[static_cast<size_t>(index)];
    this->updateLightList();
}

//------------------------------------------------------------------------------

void SLightSelector::initLightList(::fwRenderOgre::Layer::sptr layer)
{
    m_currentLayer = m_layers[0];

    if(layer == m_currentLayer.lock())
    {
        this->onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void SLightSelector::refreshLayers()
{
    m_layersBox->clear();

    ::fwServices::registry::ObjectService::ServiceVectorType renderers =
        ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

    // Fills layer combo box with all enabled layers of each render services
    for(auto srv : renderers)
    {
        ::fwRenderOgre::SRender::sptr render = ::fwRenderOgre::SRender::dynamicCast(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string id       = layerMap.first;
            const std::string renderID = render->getID();
            m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
            m_layers.push_back(layerMap.second);

            m_connections.connect(layerMap.second, ::fwRenderOgre::Layer::s_INIT_LAYER_SIG,
                                  this->getSptr(), s_INIT_LIGHT_LIST_SLOT);
        }
    }
}

//------------------------------------------------------------------------------

void SLightSelector::updateLightList()
{
    std::vector< ::fwRenderOgre::ILight::sptr > lightAdaptors =
        m_currentLayer.lock()->getLightAdaptors();

    for(auto lightAdaptor : lightAdaptors)
    {
        QString lightName = lightAdaptor->getName().c_str();

        QListWidgetItem* nextLight = new QListWidgetItem(lightName, m_lightsList);
        nextLight->setFlags(nextLight->flags() | ::Qt::ItemIsUserCheckable);
        nextLight->setCheckState(::Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
