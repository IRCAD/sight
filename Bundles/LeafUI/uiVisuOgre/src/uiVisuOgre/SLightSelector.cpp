/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SLightSelector.hpp"

#include "uiVisuOgre/helper/Utils.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <visuOgreAdaptor/SMaterial.hpp>

#include <OGRE/OgreColourValue.h>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SLightSelector, ::fwData::Composite );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SLightSelector::s_LIGHT_SELECTED_SIG = "lightSelected";
const ::fwCom::Slots::SlotKeyType SLightSelector::s_INIT_LIGHT_LIST_SLOT   = "initLightList";

//------------------------------------------------------------------------------

SLightSelector::SLightSelector() noexcept
{
    newSignal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
    newSlot(s_INIT_LIGHT_LIST_SLOT, &SLightSelector::initLightList, this);
}

//------------------------------------------------------------------------------

SLightSelector::~SLightSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SLightSelector::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());

    m_layersBox       = new QComboBox();
    m_lightsState     = new QCheckBox("Switch lights on/off");
    m_lightsList      = new QListWidget();
    m_addLightBtn     = new QPushButton("Add light");
    m_removeLightBtn  = new QPushButton("Remove light");
    m_ambientColorBtn = new QPushButton("Scene ambient color");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_lightsState);
    layout->addWidget(m_lightsList);

    QHBoxLayout* addRemoveLayout = new QHBoxLayout();
    addRemoveLayout->addWidget(m_addLightBtn);
    addRemoveLayout->addWidget(m_removeLightBtn);
    m_removeLightBtn->setEnabled(false);

    layout->addLayout(addRemoveLayout);
    layout->addWidget(m_ambientColorBtn);

    qtContainer->setLayout(layout);

    this->refreshLayers();

    this->updateLightsList();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));

    QObject::connect(m_lightsState, SIGNAL(stateChanged(int)), this, SLOT(onChangedLightsState(int)));

    QObject::connect(m_lightsList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
                     this, SLOT(onSelectedLightItem(QListWidgetItem*, QListWidgetItem*)));
    QObject::connect(m_lightsList, SIGNAL(itemChanged(QListWidgetItem*)),
                     this, SLOT(onCheckedLightItem(QListWidgetItem*)));

    QObject::connect(m_addLightBtn, SIGNAL(clicked(bool)), this, SLOT(onAddLight(bool)));
    QObject::connect(m_removeLightBtn, SIGNAL(clicked(bool)), this, SLOT(onRemoveLight(bool)));

    QObject::connect(m_ambientColorBtn, SIGNAL(clicked(bool)), this, SLOT(onEditAmbientColor(bool)));

}

//------------------------------------------------------------------------------

void SLightSelector::stopping()
{
    m_connections.disconnect();

    for(auto& lightAdaptor : m_managedLightAdaptors)
    {
        ::fwRenderOgre::ILight::destroyLightAdaptor(lightAdaptor);
    }
    m_managedLightAdaptors.clear();

    this->destroy();
}

//------------------------------------------------------------------------------

void SLightSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SLightSelector::updating()
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

void SLightSelector::onSelectedLightItem(QListWidgetItem* _item, QListWidgetItem* _previous)
{
    if(_item)
    {
        m_currentLight = this->retrieveLightAdaptor(_item->text().toStdString());

        auto sig = this->signal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
        sig->asyncEmit(m_currentLight);

        m_removeLightBtn->setEnabled(true);
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
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    ::uiVisuOgre::NewLightDialog* lightDialog = new ::uiVisuOgre::NewLightDialog(container);

    if(lightDialog->exec() == QDialog::Accepted)
    {
        std::string lightName = lightDialog->property("lightName").toString().toStdString();

        auto existingLight =
            std::find_if(m_lightAdaptors.begin(), m_lightAdaptors.end(),
                         [lightName](::fwRenderOgre::ILight::sptr lightAdaptor)
            {
                return lightAdaptor->getName() == lightName;
            });

        if(existingLight == m_lightAdaptors.end())
        {
            this->createLightAdaptor(lightName);
        }
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onRemoveLight(bool _checked)
{
    ::fwRenderOgre::ILight::destroyLightAdaptor(m_currentLight);

    if(m_currentLight)
    {
        ::fwRenderOgre::Layer::sptr currentLayer = m_currentLayer.lock();
        m_currentLight.reset();

        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        m_removeLightBtn->setEnabled(false);
        currentLayer->requestRender();
    }
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

    // Default to the first layer
    if(!m_layers.empty())
    {
        m_currentLayer  = m_layers[0];
        m_lightAdaptors = m_currentLayer.lock()->getLightAdaptors();
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

void SLightSelector::createLightAdaptor(const std::string& _name)
{
    ::fwRenderOgre::Layer::sptr currentLayer = m_currentLayer.lock();

    if(currentLayer)
    {
        ::fwData::TransformationMatrix3D::sptr lightTransform = ::fwData::TransformationMatrix3D::New();
        ::fwData::Color::sptr lightDiffuseColor               = ::fwData::Color::New();
        ::fwData::Color::sptr lightSpecularColor              = ::fwData::Color::New();

        ::fwRenderOgre::ILight::sptr lightAdaptor = ::fwRenderOgre::ILight::createLightAdaptor(lightTransform,
                                                                                               lightDiffuseColor,
                                                                                               lightSpecularColor);
        lightAdaptor->setName(_name);
        lightAdaptor->setType(::Ogre::Light::LT_DIRECTIONAL);
        lightAdaptor->setLayerID(currentLayer->getLayerID());
        lightAdaptor->setRenderService(currentLayer->getRenderService());
        lightAdaptor->start();

        m_managedLightAdaptors.push_back(lightAdaptor);
        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        ::fwServices::registry::ObjectService::ServiceVectorType materialServices =
            ::fwServices::OSR::getServices("::visuOgreAdaptor::SMaterial");

        for(auto srv : materialServices)
        {
            ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = ::visuOgreAdaptor::SMaterial::dynamicCast(srv);

            if(materialAdaptor->getLayerID() == currentLayer->getLayerID())
            {
                // Update materials of the scene to take the new light into account
                materialAdaptor->update();
            }
        }
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

NewLightDialog::NewLightDialog(QWidget* parent) :
    QDialog(parent)
{
    m_lightNameLbl  = new QLabel("Name :", this);
    m_lightNameEdit = new QLineEdit(this);

    m_okBtn = new QPushButton("Ok", this);

    QHBoxLayout* lightNameLayout = new QHBoxLayout();
    lightNameLayout->addWidget(m_lightNameLbl);
    lightNameLayout->addWidget(m_lightNameEdit);

    QVBoxLayout* newLightLayout = new QVBoxLayout();
    newLightLayout->addLayout(lightNameLayout);
    newLightLayout->addWidget(m_okBtn);

    this->setWindowTitle("New light");
    this->setModal(true);
    this->setLayout(newLightLayout);

    QObject::connect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtn(bool)));
}

//------------------------------------------------------------------------------

NewLightDialog::~NewLightDialog()
{
    QObject::disconnect(m_okBtn, SIGNAL(clicked(bool)), this, SLOT(onOkBtn(bool)));
}

//------------------------------------------------------------------------------

void NewLightDialog::onOkBtn(bool _checked)
{
    if(!m_lightNameEdit->text().isEmpty())
    {
        this->setProperty("lightName", QVariant(m_lightNameEdit->text()));
        this->accept();
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
