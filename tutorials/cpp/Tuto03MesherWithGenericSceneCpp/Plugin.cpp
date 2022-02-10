/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "Plugin.hpp"

#include <core/com/Slot.hxx>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <service/registry/Proxy.hpp>

using namespace sight;

namespace Tuto03MesherWithGenericSceneCpp
{

SIGHT_REGISTER_PLUGIN("Tuto03MesherWithGenericSceneCpp::Plugin");

static const std::string s_IMAGE_ID          = "image";
static const std::string s_IMAGE_SERIES_ID   = "imageSeries";
static const std::string s_MODEL_SERIES_ID   = "modelSeries";
static const std::string s_RECONSTRUCTION_ID = "reconstruction";

static const std::string s_REC_SELECTED_CHANNEL    = "recSelected";
static const std::string s_EMPTY_SELECTION_CHANNEL = "emptySelection";

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    m_appManager = std::unique_ptr<service::AppManager>(new service::AppManager);
    m_appManager->create();
    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/
    // GUI
    auto frameSrv = m_appManager->addService("sight::module::ui::base::SFrame", true, false);
    auto menuBar  = m_appManager->addService(
        "sight::module::ui::base::SMenuBar",
        "menuBar",
        true,
        false
    );
    auto menuFile =
        m_appManager->addService("sight::module::ui::base::SMenu", "menuFile", true, false);
    auto menuMesher = m_appManager->addService(
        "sight::module::ui::base::SMenu",
        "menuMesher",
        true,
        false
    );
    auto mainView = m_appManager->addService(
        "sight::module::ui::base::SView",
        "mainView",
        true,
        false
    );
    auto multiViewOrgans = m_appManager->addService(
        "sight::module::ui::base::SView",
        "multiViewOrgans",
        true,
        false
    );
    auto sceneEditorsView = m_appManager->addService(
        "sight::module::ui::base::SView",
        "scenesceneEditorsView",
        true,
        false
    );

    // actions
    auto actionOpenImage = m_appManager->addService(
        "sight::module::ui::base::com::SStarter",
        "actionOpenImage",
        true,
        false
    );
    auto actionSaveModelSeries = m_appManager->addService(
        "sight::module::ui::base::com::SStarter",
        "actionSaveModelSeries",
        false,
        false
    );
    auto actionCreateMesh50 = m_appManager->addService(
        "sight::module::ui::base::com::SStarter",
        "actionCreateMesh50",
        true,
        false
    );
    auto actionCreateMesh80 = m_appManager->addService(
        "sight::module::ui::base::com::SStarter",
        "actionCreateMesh80",
        true,
        false
    );
    auto actionQuit = m_appManager->addService("sight::module::ui::base::SQuit", "actionQuit", true, false);

    //readers/writers
    auto imageSeriesReader = m_appManager->addService(
        "sight::module::ui::base::io::SSelector",
        "imageSeriesReader",
        true,
        false
    );
    auto modelSeriesWriter = m_appManager->addService(
        "sight::module::ui::base::io::SSelector",
        "modelSeriesWriter",
        true,
        false
    );

    // extrator/converter
    auto extractImage = m_appManager->addService("sight::module::data::SExtractObj", true, false);

    //editors
    auto snapshotAdp = m_appManager->addService(
        "sight::module::viz::scene3d::adaptor::SFragmentsInfo",
        "snapshotAdp",
        true,
        false
    );
    {
        service::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        config.add("config.<xmlattr>.flip", "true");
        snapshotAdp->configure(config);
    }

    auto showScanEditor = m_appManager->addService(
        "sight::module::ui::qt::com::SSignalButton",
        "showScanEditor",
        true,
        false
    );
    auto sliderIndexEditor = m_appManager->addService(
        "sight::module::ui::qt::image::SliceIndexPositionEditor",
        "sliderIndexEditor",
        true,
        false
    );
    auto listOrganEditor = m_appManager->addService(
        "sight::module::ui::qt::model::SModelSeriesList",
        "listOrganEditor",
        true,
        false
    );
    auto organMaterialEditor = m_appManager->addService(
        "sight::module::ui::qt::reconstruction::SOrganMaterialEditor",
        "organMaterialEditor",
        true,
        false
    );
    auto representationEditor = m_appManager->addService(
        "sight::module::ui::qt::reconstruction::RepresentationEditor",
        "representationEditor",
        true,
        false
    );
    auto progressBar = m_appManager->addService("sight::module::ui::base::SJobBar", true, false);

    // meshers
    auto mesher50 = m_appManager->addService("sight::module::filter::mesh::SVTKMesher", "mesher50", true, false);
    auto mesher80 = m_appManager->addService("sight::module::filter::mesh::SVTKMesher", "mesher80", true, false);

    // generic scene
    auto renderSrv    = m_appManager->addService("sight::viz::scene3d::SRender", "genericScene", true, false);
    auto imageAdaptor = m_appManager->addService(
        "sight::module::viz::scene3d::adaptor::SNegato3D",
        "imageAdaptor",
        true,
        false
    );

    auto modelSeriesAdaptor = m_appManager->addService(
        "sight::module::viz::scene3d::adaptor::SModelSeries",
        "modelSeriesAdaptor",
        true,
        false
    );
    auto snapshotAdaptor = m_appManager->addService(
        "sight::module::viz::scene3d::adaptor::SFragmentsInfo",
        "snapshotAdaptor",
        true,
        false
    );
    auto trackballInteractorAdp = m_appManager->addService(
        "sight::module::viz::scene3d::adaptor::STrackballCamera",
        "trackballInteractorAdp",
        true,
        false
    );

    // Objects declaration
    auto imageSeries = data::ImageSeries::New();
    auto snapshot    = data::Image::New();

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    service::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto03MesherWithGenericSceneCpp");
    frameConfig.put("gui.frame.icon", "Tuto03MesherWithGenericSceneCpp/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameSrv->configure(frameConfig);

    // create the menuBar configuration
    service::IService::ConfigType menuBarConfig;
    service::IService::ConfigType menuBarMenu1Config;
    menuBarMenu1Config.put("<xmlattr>.name", "File");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu1Config);
    service::IService::ConfigType menuBarMenu2Config;
    menuBarMenu2Config.put("<xmlattr>.name", "Mesher");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu2Config);
    service::IService::ConfigType menuBarMenu1ConfigReg;
    menuBarMenu1ConfigReg.put("<xmlattr>.sid", "menuFile");
    menuBarConfig.add_child("registry.menu", menuBarMenu1ConfigReg);
    service::IService::ConfigType menuBarMenu2ConfigReg;
    menuBarMenu2ConfigReg.put("<xmlattr>.sid", "menuMesher");
    menuBarConfig.add_child("registry.menu", menuBarMenu2ConfigReg);
    menuBar->configure(menuBarConfig);

    // menu File
    service::IService::ConfigType menuFileConfig;
    service::IService::ConfigType menuFileItem1;
    menuFileItem1.put("<xmlattr>.name", "Open image");
    menuFileItem1.put("<xmlattr>.shortcut", "Ctrl+O");
    service::IService::ConfigType menuFileItem2;
    menuFileItem2.put("<xmlattr>.name", "Save meshes");
    menuFileItem2.put("<xmlattr>.shortcut", "Ctrl+S");
    service::IService::ConfigType menuFileItem3;
    menuFileItem3.put("<xmlattr>.name", "Quit");
    menuFileItem3.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuFileItem3.put("<xmlattr>.specialAction", "QUIT");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem1);
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem2);
    menuFileConfig.add("gui.layout", "separator");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem3);

    service::IService::ConfigType menuFileItem1Reg;
    menuFileItem1Reg.put("<xmlattr>.sid", "actionOpenImage");
    service::IService::ConfigType menuFileItem2Reg;
    menuFileItem2Reg.put("<xmlattr>.sid", "actionSaveModelSeries");
    service::IService::ConfigType menuFileItem3Reg;
    menuFileItem3Reg.put("<xmlattr>.sid", "actionQuit");
    menuFileConfig.add_child("registry.menuItem", menuFileItem1Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem2Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem3Reg);
    menuFile->configure(menuFileConfig);

    // menu mesher
    service::IService::ConfigType menuMesherConfig;
    service::IService::ConfigType menuMesherItem1;
    menuMesherItem1.put("<xmlattr>.name", "Create Mesh 50");
    service::IService::ConfigType menuMesherItem2;
    menuMesherItem2.put("<xmlattr>.name", "Create Mesh 80");
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem1);
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem2);

    service::IService::ConfigType menuMesherItem1Reg;
    menuMesherItem1Reg.put("<xmlattr>.sid", "actionCreateMesh50");
    service::IService::ConfigType menuMesherItem2Reg;
    menuMesherItem2Reg.put("<xmlattr>.sid", "actionCreateMesh80");
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem1Reg);
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem2Reg);
    menuMesher->configure(menuMesherConfig);

    // mainview
    service::IService::ConfigType mainViewConfig;
    service::IService::ConfigType mainViewLayoutConfig;
    mainViewLayoutConfig.put("<xmlattr>.type", "sight::ui::base::CardinalLayoutManager");
    service::IService::ConfigType mainView1Config;
    mainView1Config.put("<xmlattr>.align", "center");
    service::IService::ConfigType mainView2Config;
    mainView2Config.put("<xmlattr>.align", "right");
    mainView2Config.put("<xmlattr>.minWidth", "300");
    service::IService::ConfigType mainView3Config;
    mainView3Config.put("<xmlattr>.align", "bottom");
    mainView3Config.put("<xmlattr>.minHeight", "30");
    mainViewLayoutConfig.add_child("view", mainView1Config);
    mainViewLayoutConfig.add_child("view", mainView2Config);
    mainViewLayoutConfig.add_child("view", mainView3Config);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    service::IService::ConfigType mainView1ConfigReg;
    mainView1ConfigReg.put("<xmlattr>.sid", "genericScene");
    mainViewConfig.add_child("registry.view", mainView1ConfigReg);
    service::IService::ConfigType mainView2ConfigReg;
    mainView2ConfigReg.put("<xmlattr>.sid", "multiViewOrgans");
    mainViewConfig.add_child("registry.view", mainView2ConfigReg);
    service::IService::ConfigType mainView3ConfigReg;
    mainView3ConfigReg.put("<xmlattr>.sid", "scenesceneEditorsView");
    mainViewConfig.add_child("registry.view", mainView3ConfigReg);
    mainView->configure(mainViewConfig);

    // multiViewOrgans
    service::IService::ConfigType multiViewOrgansConfig;
    service::IService::ConfigType multiViewOrgansLayoutConfig;
    multiViewOrgansLayoutConfig.put("<xmlattr>.type", "sight::ui::base::ToolboxLayoutManager");
    service::IService::ConfigType multiViewOrgans1Config;
    multiViewOrgans1Config.put("<xmlattr>.caption", "Organs");
    multiViewOrgans1Config.put("<xmlattr>.expanded", true);
    service::IService::ConfigType multiViewOrgans2Config;
    multiViewOrgans2Config.put("<xmlattr>.caption", "Material");
    multiViewOrgans2Config.put("<xmlattr>.expanded", true);
    service::IService::ConfigType multiViewOrgans3Config;
    multiViewOrgans3Config.put("<xmlattr>.caption", "Representation");
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans1Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans2Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans3Config);
    multiViewOrgansConfig.add_child("gui.layout", multiViewOrgansLayoutConfig);
    service::IService::ConfigType multiViewOrgans1ConfigReg;
    multiViewOrgans1ConfigReg.put("<xmlattr>.sid", "listOrganEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans1ConfigReg);
    service::IService::ConfigType multiViewOrgans2ConfigReg;
    multiViewOrgans2ConfigReg.put("<xmlattr>.sid", "organMaterialEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans2ConfigReg);
    service::IService::ConfigType multiViewOrgans3ConfigReg;
    multiViewOrgans3ConfigReg.put("<xmlattr>.sid", "representationEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans3ConfigReg);
    multiViewOrgans->configure(multiViewOrgansConfig);

    // sceneEditorsView
    service::IService::ConfigType sceneEditorsViewConfig;
    service::IService::ConfigType sceneEditorsViewLayoutConfig;
    sceneEditorsViewLayoutConfig.put("<xmlattr>.type", "sight::ui::base::LineLayoutManager");
    service::IService::ConfigType editorsOrientation;
    editorsOrientation.put("<xmlattr>.value", "horizontal");
    sceneEditorsViewLayoutConfig.add_child("orientation", editorsOrientation);
    service::IService::ConfigType sceneEditorsView2;
    sceneEditorsView2.put("<xmlattr>.proportion", "0");
    sceneEditorsView2.put("<xmlattr>.minWidth", "50");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView2);
    service::IService::ConfigType sceneEditorsView3;
    sceneEditorsView3.put("<xmlattr>.proportion", "1");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView3);
    service::IService::ConfigType sceneEditorsView4;
    sceneEditorsView4.put("<xmlattr>.proportion", "0");
    sceneEditorsView4.put("<xmlattr>.minWidth", "30");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView4);
    menuFileConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    sceneEditorsViewConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    service::IService::ConfigType sceneEditorsView2Reg;
    sceneEditorsView2Reg.put("<xmlattr>.sid", "showScanEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView2Reg);
    service::IService::ConfigType sceneEditorsView3Reg;
    sceneEditorsView3Reg.put("<xmlattr>.sid", "sliderIndexEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView3Reg);
    service::IService::ConfigType sceneEditorsView4Reg;
    sceneEditorsView4Reg.put("<xmlattr>.sid", "snapshotEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView4Reg);
    sceneEditorsView->configure(sceneEditorsViewConfig);

    /* **************************************************************************************
    *              actions configuration
    ****************************************************************************************/

    service::IService::ConfigType actionOpenImageConfig;
    actionOpenImageConfig.add("start.<xmlattr>.uid", "imageSeriesReader");
    actionOpenImage->configure(actionOpenImageConfig);

    service::IService::ConfigType actionSaveModelSeriesConfig;
    actionSaveModelSeriesConfig.add("start.<xmlattr>.uid", "meshReader");
    actionSaveModelSeries->configure(actionSaveModelSeriesConfig);

    service::IService::ConfigType actionCreateMesh50Config;
    actionCreateMesh50Config.add("state.<xmlattr>.executable", false);
    actionCreateMesh50Config.add("start.<xmlattr>.uid", "mesher50");
    actionCreateMesh50->configure(actionCreateMesh50Config);

    service::IService::ConfigType actionCreateMesh80Config;
    actionCreateMesh80Config.add("state.<xmlattr>.executable", false);
    actionCreateMesh80Config.add("start.<xmlattr>.uid", "mesher80");
    actionCreateMesh80->configure(actionCreateMesh80Config);

    actionQuit->configure();

    /* **************************************************************************************
    *              readers/writers configuration
    ****************************************************************************************/

    service::IService::ConfigType imageSeriesReaderConfig;
    imageSeriesReaderConfig.put("type.<xmlattr>.mode", "reader");
    imageSeriesReaderConfig.put("type.<xmlattr>.class", "sight::data::ImageSeries");
    imageSeriesReader->configure(imageSeriesReaderConfig);

    service::IService::ConfigType modelSeriesWriterConfig;
    modelSeriesWriterConfig.put("type.<xmlattr>.mode", "writer");
    modelSeriesWriter->configure(modelSeriesWriterConfig);

    /* **************************************************************************************
    *              extractor configuration
    ****************************************************************************************/

    service::IService::ConfigType extractImageConfig;
    extractImageConfig.put("inout.extract.<xmlattr>.from", "@image");
    extractImage->configure(extractImageConfig);

    /* **************************************************************************************
    *              editors configuration
    ****************************************************************************************/

    service::IService::ConfigType showScanEditorConfig;
    showScanEditorConfig.add("config.checkable", true);
    showScanEditorConfig.add("config.icon", "sight::module::ui::flaticons/RedCross.svg");
    showScanEditorConfig.add("config.icon2", "sight::module::ui::flaticons/Layers.svg");
    showScanEditorConfig.add("config.iconWidth", "40");
    showScanEditorConfig.add("config.iconHeight", "16");
    showScanEditorConfig.add("config.checked", true);
    showScanEditor->configure(showScanEditorConfig);

    service::IService::ConfigType sliderIndexEditorConfig;
    sliderIndexEditorConfig.put("sliceIndex", "axial");
    sliderIndexEditor->configure(sliderIndexEditorConfig);

    service::IService::ConfigType listOrganEditorConfig;
    listOrganEditorConfig.add("columns.organ_name", "@organ_name");
    listOrganEditor->configure(listOrganEditorConfig);

    organMaterialEditor->configure();
    representationEditor->configure();
    snapshotAdp->configure();

    /* **************************************************************************************
    *              genericScene configuration
    ****************************************************************************************/

    // create and register the render service
    service::IService::ConfigType renderConfig;

    renderConfig.add("scene.background.<xmlattr>.topColor", "#36393E");
    renderConfig.add("scene.background.<xmlattr>.bottomColor", "#36393E");
    renderConfig.add("scene.layer.<xmlattr>.id", "default");
    renderConfig.add("scene.layer.<xmlattr>.order", "1");

    service::IService::ConfigType adpt1Config;
    adpt1Config.put("<xmlattr>.uid", "modelSeriesAdaptor");
    renderConfig.add_child("scene.adaptor", adpt1Config);
    service::IService::ConfigType adpt2Config;
    adpt2Config.put("<xmlattr>.uid", "imageAdaptor");
    renderConfig.add_child("scene.adaptor", adpt2Config);
    service::IService::ConfigType adpt3Config;
    adpt3Config.put("<xmlattr>.uid", "snapshotAdaptor");
    renderConfig.add_child("scene.adaptor", adpt3Config);
    service::IService::ConfigType adpt4Config;
    adpt4Config.put("<xmlattr>.uid", "trackballInteractorAdp");
    renderConfig.add_child("scene.adaptor", adpt4Config);
    renderSrv->configure(renderConfig);

    service::IService::ConfigType imageAdaptorConfig;
    imageAdaptorConfig.add("config.<xmlattr>.layer", "default");
    imageAdaptorConfig.add("config.<xmlattr>.mode", "3d");
    imageAdaptorConfig.add("config.<xmlattr>.slice", "3");
    imageAdaptorConfig.add("config.<xmlattr>.sliceIndex", "axial");
    imageAdaptor->configure(imageAdaptorConfig);

    service::IService::ConfigType modelSeriesAdaptorConfig;
    modelSeriesAdaptorConfig.add("config.<xmlattr>.layer", "default");
    modelSeriesAdaptorConfig.add("config.<xmlattr>.picker", "");
    modelSeriesAdaptor->configure(modelSeriesAdaptorConfig);

    service::IService::ConfigType snapshotAdaptorConfig;
    snapshotAdaptorConfig.add("config.<xmlattr>.layer", "default");
    snapshotAdaptor->configure(snapshotAdaptorConfig);

    service::IService::ConfigType trackballInteractorAdpConfig;
    trackballInteractorAdpConfig.add("config.<xmlattr>.layer", "default");
    trackballInteractorAdp->configure(trackballInteractorAdpConfig);

    /* **************************************************************************************
    *              register inputs/inouts
    ****************************************************************************************/

    imageSeriesReader->setInOut(imageSeries, "data");
    snapshotAdp->setInOut(snapshot, "image", true);
    extractImage->setInOut(imageSeries, "source");
    mesher50->setInput(imageSeries, "imageSeries");
    mesher80->setInput(imageSeries, "imageSeries");

    modelSeriesWriter->setObjectId("data", s_MODEL_SERIES_ID);
    sliderIndexEditor->setObjectId("image", s_IMAGE_ID);
    imageAdaptor->setObjectId("image", s_IMAGE_ID);
    extractImage->setObjectId("target", s_IMAGE_ID, 0);
    listOrganEditor->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    organMaterialEditor->setObjectId("reconstruction", s_RECONSTRUCTION_ID);
    representationEditor->setObjectId("reconstruction", s_RECONSTRUCTION_ID);
    mesher50->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    mesher80->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    modelSeriesAdaptor->setObjectId("model", s_MODEL_SERIES_ID);

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    auto worker = core::thread::Worker::New();
    core::thread::addWorker("Tuto09", worker);

    auto proxy = service::registry::Proxy::getDefault();

    std::function<void(data::Object::sptr)> recSelectedFct =
        [ = ](data::Object::sptr rec)
        {
            m_appManager->addObject(rec, s_RECONSTRUCTION_ID);
        };
    m_slotRecSelected = core::com::newSlot(recSelectedFct);
    m_slotRecSelected->setWorker(worker);
    proxy->connect(s_REC_SELECTED_CHANNEL, m_slotRecSelected);

    std::function<void()> emptySelectionFct =
        [ = ]()
        {
            m_appManager->removeObject(nullptr, s_RECONSTRUCTION_ID);
        };
    m_slotEmptySelection = core::com::newSlot(emptySelectionFct);
    m_slotEmptySelection->setWorker(worker);
    proxy->connect(s_EMPTY_SELECTION_CHANNEL, m_slotEmptySelection);

    service::helper::ProxyConnections jobCnt;
    jobCnt.addSignalConnection(imageSeriesReader->getID(), "jobCreated");
    jobCnt.addSignalConnection(modelSeriesWriter->getID(), "jobCreated");
    jobCnt.addSlotConnection(progressBar->getID(), "showJob");
    m_appManager->addProxyConnection(jobCnt);

    service::helper::ProxyConnections extractCnt;
    extractCnt.addSignalConnection(imageSeries->getID(), "modified");
    extractCnt.addSlotConnection(extractImage->getID(), "update");
    m_appManager->addProxyConnection(extractCnt);

    service::helper::ProxyConnections showScanCnt;
    showScanCnt.addSignalConnection(showScanEditor->getID(), "toggled");
    showScanCnt.addSlotConnection(imageAdaptor->getID(), "updateVisibility");
    m_appManager->addProxyConnection(showScanCnt);

    service::helper::ProxyConnections recSelectedCnt(s_REC_SELECTED_CHANNEL);
    recSelectedCnt.addSignalConnection(listOrganEditor->getID(), "reconstructionSelected");
    m_appManager->addProxyConnection(recSelectedCnt);
    service::helper::ProxyConnections emptySelectionCnt(s_EMPTY_SELECTION_CHANNEL);
    emptySelectionCnt.addSignalConnection(listOrganEditor->getID(), "emptiedSelection");
    m_appManager->addProxyConnection(emptySelectionCnt);

    service::helper::ProxyConnections mesherOnCnt;
    mesherOnCnt.addSignalConnection(mesher50->getID(), "started");
    mesherOnCnt.addSignalConnection(mesher80->getID(), "started");
    mesherOnCnt.addSlotConnection(actionCreateMesh50->getID(), "setExecutable");
    mesherOnCnt.addSlotConnection(actionCreateMesh80->getID(), "setExecutable");
    m_appManager->addProxyConnection(mesherOnCnt);

    service::helper::ProxyConnections mesherOffCnt;
    mesherOffCnt.addSignalConnection(mesher50->getID(), "stopped");
    mesherOffCnt.addSignalConnection(mesher80->getID(), "stopped");
    mesherOffCnt.addSlotConnection(actionCreateMesh50->getID(), "setInexecutable");
    mesherOffCnt.addSlotConnection(actionCreateMesh80->getID(), "setInexecutable");
    m_appManager->addProxyConnection(mesherOffCnt);

    m_appManager->addObject(imageSeries, imageSeries->getID());
    m_appManager->addObject(snapshot, snapshot->getID());

    /* **************************************************************************************
    *              start the services
    ****************************************************************************************/

    m_appManager->startServices();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    auto proxy = service::registry::Proxy::getDefault();

    proxy->disconnect(s_REC_SELECTED_CHANNEL, m_slotRecSelected);
    proxy->disconnect(s_EMPTY_SELECTION_CHANNEL, m_slotEmptySelection);

    m_appManager->destroy();
    m_appManager.reset();

    core::thread::removeWorker("Tuto09");
}

//------------------------------------------------------------------------------

} // namespace Tuto03MesherWithGenericSceneCpp
