#include "uiVisuOgre/SLightSelector.hpp"

#include "uiVisuOgre/helper/Utils.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <OGRE/OgreColourValue.h>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SLightSelector, ::fwData::Composite );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SLightSelector::s_LIGHT_SELECTED_SIG = "lightSelected";
const ::fwCom::Slots::SlotKeyType SLightSelector::s_INIT_LIGHT_LIST_SLOT   = "initLightList";

//------------------------------------------------------------------------------

SLightSelector::SLightSelector() throw()
{
    newSignal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
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

    m_layersBox       = new QComboBox(container);
    m_lightsState     = new QCheckBox("Switch lights on/off", container);
    m_lightsList      = new QListWidget(container);
    m_addLightBtn     = new QPushButton("Add light", container);
    m_ambientColorBtn = new QPushButton("Scene ambient color", container);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_lightsState);
    layout->addWidget(m_lightsList);

    QHBoxLayout* addRemoveLayout = new QHBoxLayout();
    addRemoveLayout->addWidget(m_addLightBtn);

    layout->addLayout(addRemoveLayout);
    layout->addWidget(m_ambientColorBtn);

    container->setLayout(layout);

    this->refreshLayers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));

    QObject::connect(m_lightsState, SIGNAL(stateChanged(int)), this, SLOT(onChangedLightsState(int)));

    QObject::connect(m_lightsList, SIGNAL(itemActivated(QListWidgetItem*)),
                     this, SLOT(onSelectedLightItem(QListWidgetItem*)));
    QObject::connect(m_lightsList, SIGNAL(itemChanged(QListWidgetItem*)),
                     this, SLOT(onCheckedLightItem(QListWidgetItem*)));

    QObject::connect(m_addLightBtn, SIGNAL(clicked(bool)), this, SLOT(onAddLight(bool)));

    QObject::connect(m_ambientColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditAmbientColor(bool)));
}

//------------------------------------------------------------------------------

void SLightSelector::stopping() throw(::fwTools::Failed)
{
    m_connections.disconnect();

    QObject::disconnect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));

    QObject::disconnect(m_lightsState, SIGNAL(stateChanged(int)), this, SLOT(onChangedLightsState(int)));

    QObject::disconnect(m_lightsList, SIGNAL(itemActivated(QListWidgetItem*)),
                        this, SLOT(onSelectedLightItem(QListWidgetItem*)));
    QObject::disconnect(m_lightsList, SIGNAL(itemChanged(QListWidgetItem*)),
                        this, SLOT(onCheckedLightItem(QListWidgetItem*)));

    QObject::disconnect(m_addLightBtn, SIGNAL(clicked(bool)), this, SLOT(onAddLight(bool)));

    QObject::disconnect(m_ambientColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditAmbientColor(bool)));

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

void SLightSelector::onSelectedLayerItem(int _index)
{
    m_currentLayer  = m_layers[static_cast<size_t>(_index)];
    m_lightAdaptors = m_currentLayer.lock()->getLightAdaptors();

    this->updateLightsList();
}

//------------------------------------------------------------------------------

void SLightSelector::onChangedLightsState(int _state)
{
    for(int i(0); i < m_lightsList->count(); ++i)
    {
        QListWidgetItem* item = m_lightsList->item(i);
        item->setCheckState(static_cast< ::Qt::CheckState >(_state));
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onSelectedLightItem(QListWidgetItem* _item)
{
    if(_item->checkState() == ::Qt::Checked)
    {
        m_currentLight = this->retrieveLightAdaptor(_item->text().toStdString());

        auto sig = this->signal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
        sig->asyncEmit(m_currentLight);
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onCheckedLightItem(QListWidgetItem* _item)
{
    ::fwRenderOgre::ILight::sptr checkedLightAdaptor =
        this->retrieveLightAdaptor(_item->text().toStdString());

    checkedLightAdaptor->switchOn(_item->checkState() == ::Qt::Checked);
}

//------------------------------------------------------------------------------

void SLightSelector::onAddLight(bool _checked)
{
    //TODO: Implement light adding method.
}

//------------------------------------------------------------------------------

void SLightSelector::onEditAmbientColor(bool _checked)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();
    ::Ogre::ColourValue ogreColor     = layer->getSceneManager()->getAmbientLight();

    QColor qColor = QColorDialog::getColor(::uiVisuOgre::helper::Utils::converOgreColorToQColor(ogreColor),
                                           container,
                                           "Scene ambient color");

    ogreColor = ::uiVisuOgre::helper::Utils::convertQColorToOgreColor(qColor);

    layer->getSceneManager()->setAmbientLight(ogreColor);
    layer->requestRender();
}

//------------------------------------------------------------------------------

void SLightSelector::initLightList(::fwRenderOgre::Layer::sptr _layer)
{
    m_currentLayer = m_layers[0];

    if(_layer == m_currentLayer.lock())
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

void SLightSelector::updateLightsList()
{
    m_lightsList->clear();

    for(auto lightAdaptor : m_lightAdaptors)
    {
        QString lightName = lightAdaptor->getName().c_str();
        ::Qt::CheckState lightState = lightAdaptor->isSwitchedOn() ? ::Qt::Checked :
                                      ::Qt::Unchecked;

        QListWidgetItem* nextLight = new QListWidgetItem(lightName, m_lightsList);
        nextLight->setFlags(nextLight->flags() | ::Qt::ItemIsUserCheckable);
        nextLight->setCheckState(lightState);
    }
}

//------------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr SLightSelector::retrieveLightAdaptor(const std::string& _name) const
{
    auto it = std::find_if(m_lightAdaptors.begin(), m_lightAdaptors.end(),
                           [_name](::fwRenderOgre::ILight::sptr lightAdaptor)
        {
            return lightAdaptor->getName() == _name;
        });

    return it != m_lightAdaptors.end() ? *it : nullptr;
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
