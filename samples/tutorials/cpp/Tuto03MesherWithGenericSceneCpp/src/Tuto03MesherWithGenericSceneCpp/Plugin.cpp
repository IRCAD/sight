/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "Tuto03MesherWithGenericSceneCpp/Plugin.hpp"

#include <core/com/Slot.hxx>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <services/registry/ActiveWorkers.hpp>
#include <services/registry/Proxy.hpp>

namespace Tuto03MesherWithGenericSceneCpp
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::Tuto03MesherWithGenericSceneCpp::Plugin");

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
    m_appManager = std::unique_ptr< services::AppManager >(new services::AppManager );
    m_appManager->create();

    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/
    // GUI
    auto frameSrv = m_appManager->addService("::modules::ui::base::frame::SDefaultFrame", true, false);
    auto menuBar  = m_appManager->addService("::modules::ui::base::aspect::SDefaultMenuBar", "menuBar", true,
                                             false);
    auto menuFile =
        m_appManager->addService("::modules::ui::base::aspect::SDefaultMenu", "menuFile", true, false);
    auto menuMesher = m_appManager->addService("::modules::ui::base::aspect::SDefaultMenu", "menuMesher", true,
                                               false);
    auto mainView        = m_appManager->addService("::modules::ui::base::view::SDefaultView", "mainView", true, false);
    auto multiViewOrgans = m_appManager->addService("::modules::ui::base::view::SDefaultView", "multiViewOrgans", true,
                                                    false);
    auto sceneEditorsView = m_appManager->addService("::modules::ui::base::view::SDefaultView", "scenesceneEditorsView",
                                                     true, false);

    // actions
    auto actionOpenImage = m_appManager->addService("::modules::ui::base::action::SStarter", "actionOpenImage", true,
                                                    false);
    auto actionSaveModelSeries = m_appManager->addService("::modules::ui::base::action::SStarter",
                                                          "actionSaveModelSeries",
                                                          false,
                                                          false);
    auto actionCreateMesh50 = m_appManager->addService("::modules::ui::base::action::SStarter", "actionCreateMesh50",
                                                       true,
                                                       false);
    auto actionCreateMesh80 = m_appManager->addService("::modules::ui::base::action::SStarter", "actionCreateMesh80",
                                                       true,
                                                       false);
    auto actionQuit = m_appManager->addService("::modules::ui::base::action::SQuit", "actionQuit", true, false);

    //readers/writers
    auto imageSeriesReader = m_appManager->addService("::uiIO::editor::SIOSelector", "imageSeriesReader", true, false);
    auto modelSeriesWriter = m_appManager->addService("::uiIO::editor::SIOSelector", "modelSeriesWriter", true, false);

    // extrator/converter
    auto extractImage         = m_appManager->addService("::ctrlCamp::SExtractObj", true, true);
    auto medicaImageConverter = m_appManager->addService("::ctrlSelection::MedicalImageSrv", true);

    //editors
    auto snapshotEditor = m_appManager->addService("::modules::ui::qt::viz::SnapshotEditor", "snapshotEditor", true,
                                                   false);
    auto sliceListEditor = m_appManager->addService("::modules::ui::qt::editor::SSelectionMenuButton",
                                                    "sliceListEditor", true, false);
    auto showScanEditor = m_appManager->addService("::modules::ui::qt::editor::SSignalButton", "showScanEditor", true,
                                                   false);
    auto sliderIndexEditor = m_appManager->addService("::modules::ui::qt::image::SliceIndexPositionEditor",
                                                      "sliderIndexEditor", true, false);
    auto listOrganEditor = m_appManager->addService("::uiMedDataQt::editor::SModelSeriesList",
                                                    "listOrganEditor", true, false);
    auto organMaterialEditor = m_appManager->addService("::modules::ui::qt::reconstruction::SOrganMaterialEditor",
                                                        "organMaterialEditor", true, false);
    auto representationEditor = m_appManager->addService("::modules::ui::qt::reconstruction::RepresentationEditor",
                                                         "representationEditor", true, false);
    auto progressBar = m_appManager->addService("::modules::ui::base::editor::SJobBar", true, false);

    // meshers
    auto mesher50 = m_appManager->addService("::opVTKMesh::SVTKMesher", "mesher50", true, false);
    auto mesher80 = m_appManager->addService("::opVTKMesh::SVTKMesher", "mesher80", true, false);

    // generic scene
    auto renderSrv          = m_appManager->addService("::fwRenderVTK::SRender", "genericScene", true, false);
    auto imageAdaptor       = m_appManager->addService("::visuVTKAdaptor::SNegatoMPR", "imageAdaptor", true, false);
    auto modelSeriesAdaptor = m_appManager->addService("::visuVTKAdaptor::SModelSeries", "modelSeriesAdaptor", true,
                                                       false);
    auto snapshotAdaptor = m_appManager->addService("::visuVTKAdaptor::SSnapshot", "snapshotAdaptor", true, false);

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    services::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto03MesherWithGenericSceneCpp");
    frameConfig.put("gui.frame.icon", "Tuto03MesherWithGenericSceneCpp-0.2/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameSrv->configure(frameConfig);

    // create the menuBar configuration
    services::IService::ConfigType menuBarConfig;
    services::IService::ConfigType menuBarMenu1Config;
    menuBarMenu1Config.put("<xmlattr>.name", "File");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu1Config);
    services::IService::ConfigType menuBarMenu2Config;
    menuBarMenu2Config.put("<xmlattr>.name", "Mesher");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu2Config);
    services::IService::ConfigType menuBarMenu1ConfigReg;
    menuBarMenu1ConfigReg.put("<xmlattr>.sid", "menuFile");
    menuBarConfig.add_child("registry.menu", menuBarMenu1ConfigReg);
    services::IService::ConfigType menuBarMenu2ConfigReg;
    menuBarMenu2ConfigReg.put("<xmlattr>.sid", "menuMesher");
    menuBarConfig.add_child("registry.menu", menuBarMenu2ConfigReg);
    menuBar->configure(menuBarConfig);

    // menu File
    services::IService::ConfigType menuFileConfig;
    services::IService::ConfigType menuFileItem1;
    menuFileItem1.put("<xmlattr>.name", "Open image");
    menuFileItem1.put("<xmlattr>.shortcut", "Ctrl+O");
    services::IService::ConfigType menuFileItem2;
    menuFileItem2.put("<xmlattr>.name", "Save meshes");
    menuFileItem2.put("<xmlattr>.shortcut", "Ctrl+S");
    services::IService::ConfigType menuFileItem3;
    menuFileItem3.put("<xmlattr>.name", "Quit");
    menuFileItem3.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuFileItem3.put("<xmlattr>.specialAction", "QUIT");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem1);
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem2);
    menuFileConfig.add("gui.layout", "separator");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem3);

    services::IService::ConfigType menuFileItem1Reg;
    menuFileItem1Reg.put("<xmlattr>.sid", "actionOpenImage");
    services::IService::ConfigType menuFileItem2Reg;
    menuFileItem2Reg.put("<xmlattr>.sid", "actionSaveModelSeries");
    services::IService::ConfigType menuFileItem3Reg;
    menuFileItem3Reg.put("<xmlattr>.sid", "actionQuit");
    menuFileConfig.add_child("registry.menuItem", menuFileItem1Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem2Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem3Reg);
    menuFile->configure(menuFileConfig);

    // menu mesher
    services::IService::ConfigType menuMesherConfig;
    services::IService::ConfigType menuMesherItem1;
    menuMesherItem1.put("<xmlattr>.name", "Create Mesh 50");
    services::IService::ConfigType menuMesherItem2;
    menuMesherItem2.put("<xmlattr>.name", "Create Mesh 80");
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem1);
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem2);

    services::IService::ConfigType menuMesherItem1Reg;
    menuMesherItem1Reg.put("<xmlattr>.sid", "actionCreateMesh50");
    services::IService::ConfigType menuMesherItem2Reg;
    menuMesherItem2Reg.put("<xmlattr>.sid", "actionCreateMesh80");
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem1Reg);
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem2Reg);
    menuMesher->configure(menuMesherConfig);

    // mainview
    services::IService::ConfigType mainViewConfig;
    services::IService::ConfigType mainViewLayoutConfig;
    mainViewLayoutConfig.put("<xmlattr>.type", "::ui::base::CardinalLayoutManager");
    services::IService::ConfigType mainView1Config;
    mainView1Config.put("<xmlattr>.align", "center");
    services::IService::ConfigType mainView2Config;
    mainView2Config.put("<xmlattr>.align", "right");
    mainView2Config.put("<xmlattr>.minWidth", "300");
    services::IService::ConfigType mainView3Config;
    mainView3Config.put("<xmlattr>.align", "bottom");
    mainView3Config.put("<xmlattr>.minHeight", "30");
    mainViewLayoutConfig.add_child("view", mainView1Config);
    mainViewLayoutConfig.add_child("view", mainView2Config);
    mainViewLayoutConfig.add_child("view", mainView3Config);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    services::IService::ConfigType mainView1ConfigReg;
    mainView1ConfigReg.put("<xmlattr>.sid", "genericScene");
    mainViewConfig.add_child("registry.view", mainView1ConfigReg);
    services::IService::ConfigType mainView2ConfigReg;
    mainView2ConfigReg.put("<xmlattr>.sid", "multiViewOrgans");
    mainViewConfig.add_child("registry.view", mainView2ConfigReg);
    services::IService::ConfigType mainView3ConfigReg;
    mainView3ConfigReg.put("<xmlattr>.sid", "scenesceneEditorsView");
    mainViewConfig.add_child("registry.view", mainView3ConfigReg);
    mainView->configure(mainViewConfig);

    // multiViewOrgans
    services::IService::ConfigType multiViewOrgansConfig;
    services::IService::ConfigType multiViewOrgansLayoutConfig;
    multiViewOrgansLayoutConfig.put("<xmlattr>.type", "::ui::base::ToolboxLayoutManager");
    services::IService::ConfigType multiViewOrgans1Config;
    multiViewOrgans1Config.put("<xmlattr>.caption", "Organs");
    multiViewOrgans1Config.put("<xmlattr>.expanded", true);
    services::IService::ConfigType multiViewOrgans2Config;
    multiViewOrgans2Config.put("<xmlattr>.caption", "Material");
    multiViewOrgans2Config.put("<xmlattr>.expanded", true);
    services::IService::ConfigType multiViewOrgans3Config;
    multiViewOrgans3Config.put("<xmlattr>.caption", "Representation");
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans1Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans2Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans3Config);
    multiViewOrgansConfig.add_child("gui.layout", multiViewOrgansLayoutConfig);
    services::IService::ConfigType multiViewOrgans1ConfigReg;
    multiViewOrgans1ConfigReg.put("<xmlattr>.sid", "listOrganEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans1ConfigReg);
    services::IService::ConfigType multiViewOrgans2ConfigReg;
    multiViewOrgans2ConfigReg.put("<xmlattr>.sid", "organMaterialEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans2ConfigReg);
    services::IService::ConfigType multiViewOrgans3ConfigReg;
    multiViewOrgans3ConfigReg.put("<xmlattr>.sid", "representationEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans3ConfigReg);
    multiViewOrgans->configure(multiViewOrgansConfig);

    // sceneEditorsView
    services::IService::ConfigType sceneEditorsViewConfig;
    services::IService::ConfigType sceneEditorsViewLayoutConfig;
    sceneEditorsViewLayoutConfig.put("<xmlattr>.type", "::ui::base::LineLayoutManager");
    services::IService::ConfigType editorsOrientation;
    editorsOrientation.put("<xmlattr>.value", "horizontal");
    sceneEditorsViewLayoutConfig.add_child("orientation", editorsOrientation);
    services::IService::ConfigType sceneEditorsView1;
    sceneEditorsView1.put("<xmlattr>.proportion", "0");
    sceneEditorsView1.put("<xmlattr>.minWidth", "30");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView1);
    services::IService::ConfigType sceneEditorsView2;
    sceneEditorsView2.put("<xmlattr>.proportion", "0");
    sceneEditorsView2.put("<xmlattr>.minWidth", "50");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView2);
    services::IService::ConfigType sceneEditorsView3;
    sceneEditorsView3.put("<xmlattr>.proportion", "1");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView3);
    services::IService::ConfigType sceneEditorsView4;
    sceneEditorsView4.put("<xmlattr>.proportion", "0");
    sceneEditorsView4.put("<xmlattr>.minWidth", "30");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView4);
    menuFileConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    sceneEditorsViewConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    services::IService::ConfigType sceneEditorsView1Reg;
    sceneEditorsView1Reg.put("<xmlattr>.sid", "sliceListEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView1Reg);
    services::IService::ConfigType sceneEditorsView2Reg;
    sceneEditorsView2Reg.put("<xmlattr>.sid", "showScanEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView2Reg);
    services::IService::ConfigType sceneEditorsView3Reg;
    sceneEditorsView3Reg.put("<xmlattr>.sid", "sliderIndexEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView3Reg);
    services::IService::ConfigType sceneEditorsView4Reg;
    sceneEditorsView4Reg.put("<xmlattr>.sid", "snapshotEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView4Reg);
    sceneEditorsView->configure(sceneEditorsViewConfig);

    /* **************************************************************************************
    *              actions configuration
    ****************************************************************************************/

    services::IService::ConfigType actionOpenImageConfig;
    actionOpenImageConfig.add("start.<xmlattr>.uid", "imageSeriesReader");
    actionOpenImage->configure(actionOpenImageConfig);

    services::IService::ConfigType actionSaveModelSeriesConfig;
    actionSaveModelSeriesConfig.add("start.<xmlattr>.uid", "meshReader");
    actionSaveModelSeries->configure(actionSaveModelSeriesConfig);

    services::IService::ConfigType actionCreateMesh50Config;
    actionCreateMesh50Config.add("state.<xmlattr>.executable", false);
    actionCreateMesh50Config.add("start.<xmlattr>.uid", "mesher50");
    actionCreateMesh50->configure(actionCreateMesh50Config);

    services::IService::ConfigType actionCreateMesh80Config;
    actionCreateMesh80Config.add("state.<xmlattr>.executable", false);
    actionCreateMesh80Config.add("start.<xmlattr>.uid", "mesher80");
    actionCreateMesh80->configure(actionCreateMesh80Config);

    actionQuit->configure();

    /* **************************************************************************************
    *              readers/writers configuration
    ****************************************************************************************/

    services::IService::ConfigType imageSeriesReaderConfig;
    imageSeriesReaderConfig.put("type.<xmlattr>.mode", "reader");
    imageSeriesReaderConfig.put("type.<xmlattr>.class", "::sight::data::ImageSeries");
    imageSeriesReader->configure(imageSeriesReaderConfig);

    services::IService::ConfigType modelSeriesWriterConfig;
    modelSeriesWriterConfig.put("type.<xmlattr>.mode", "writer");
    modelSeriesWriter->configure(modelSeriesWriterConfig);

    /* **************************************************************************************
    *              extractor configuration
    ****************************************************************************************/

    services::IService::ConfigType extractImageConfig;
    extractImageConfig.put("inout.extract.<xmlattr>.from", "@image");
    extractImage->configure(extractImageConfig);

    /* **************************************************************************************
    *              editors configuration
    ****************************************************************************************/

    services::IService::ConfigType sliceListEditorConfig;
    sliceListEditorConfig.add("toolTip", "Manage slice visibility");
    sliceListEditorConfig.add("selected", "3");
    services::IService::ConfigType sliceListEditorItem1Config;
    sliceListEditorItem1Config.put("<xmlattr>.text", "One slice");
    sliceListEditorItem1Config.put("<xmlattr>.value", "1");
    sliceListEditorConfig.add_child("items.item", sliceListEditorItem1Config);
    services::IService::ConfigType sliceListEditorItem2Config;
    sliceListEditorItem2Config.put("<xmlattr>.text", "Three slices");
    sliceListEditorItem2Config.put("<xmlattr>.value", "3");
    sliceListEditorConfig.add_child("items.item", sliceListEditorItem2Config);
    sliceListEditor->configure(sliceListEditorConfig);

    services::IService::ConfigType showScanEditorConfig;
    showScanEditorConfig.add("config.checkable", true);
    showScanEditorConfig.add("config.icon", "media-0.1/icons/sliceHide.png");
    showScanEditorConfig.add("config.icon2", "media-0.1/icons/sliceShow.png");
    showScanEditorConfig.add("config.iconWidth", "40");
    showScanEditorConfig.add("config.iconHeight", "16");
    showScanEditorConfig.add("config.checked", true);
    showScanEditor->configure(showScanEditorConfig);

    services::IService::ConfigType sliderIndexEditorConfig;
    sliderIndexEditorConfig.put("sliceIndex", "axial");
    sliderIndexEditor->configure(sliderIndexEditorConfig);

    services::IService::ConfigType listOrganEditorConfig;
    listOrganEditorConfig.add("columns.organ_name", "@organ_name");
    listOrganEditor->configure(listOrganEditorConfig);

    organMaterialEditor->configure();
    representationEditor->configure();
    snapshotEditor->configure();

    /* **************************************************************************************
    *              genericScene configuration
    ****************************************************************************************/

    // create and register the render service
    services::IService::ConfigType renderConfig;
    services::IService::ConfigType pickerConfig;
    pickerConfig.add("<xmlattr>.id", "picker");
    pickerConfig.add("<xmlattr>.vtkclass", "fwVtkCellPicker");
    renderConfig.add_child("scene.picker", pickerConfig);
    renderConfig.add("scene.renderer.<xmlattr>.id", "default");
    services::IService::ConfigType adpt1Config;
    adpt1Config.put("<xmlattr>.uid", "modelSeriesAdaptor");
    renderConfig.add_child("scene.adaptor", adpt1Config);
    services::IService::ConfigType adpt2Config;
    adpt2Config.put("<xmlattr>.uid", "imageAdaptor");
    renderConfig.add_child("scene.adaptor", adpt2Config);
    services::IService::ConfigType adpt3Config;
    adpt3Config.put("<xmlattr>.uid", "snapshotAdaptor");
    renderConfig.add_child("scene.adaptor", adpt3Config);
    renderSrv->configure(renderConfig);

    services::IService::ConfigType imageAdaptorConfig;
    imageAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    imageAdaptorConfig.add("config.<xmlattr>.picker", "picker");
    imageAdaptorConfig.add("config.<xmlattr>.mode", "3d");
    imageAdaptorConfig.add("config.<xmlattr>.slice", "3");
    imageAdaptorConfig.add("config.<xmlattr>.sliceIndex", "axial");
    imageAdaptor->configure(imageAdaptorConfig);

    services::IService::ConfigType modelSeriesAdaptorConfig;
    modelSeriesAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    modelSeriesAdaptorConfig.add("config.<xmlattr>.picker", "");
    modelSeriesAdaptor->configure(modelSeriesAdaptorConfig);

    services::IService::ConfigType snapshotAdaptorConfig;
    snapshotAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    snapshotAdaptor->configure(snapshotAdaptorConfig);

    /* **************************************************************************************
    *              register inputs/inouts
    ****************************************************************************************/

    imageSeriesReader->setObjectId("data", s_IMAGE_SERIES_ID);
    modelSeriesWriter->setObjectId("data", s_MODEL_SERIES_ID);
    sliderIndexEditor->setObjectId("image", s_IMAGE_ID);
    imageAdaptor->setObjectId("image", s_IMAGE_ID);
    extractImage->setObjectId("source", s_IMAGE_SERIES_ID);
    extractImage->setObjectId("target", 0, s_IMAGE_ID);
    medicaImageConverter->setObjectId( "image", s_IMAGE_ID);
    listOrganEditor->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    organMaterialEditor->setObjectId("reconstruction", s_RECONSTRUCTION_ID);
    representationEditor->setObjectId("reconstruction", s_RECONSTRUCTION_ID);
    mesher50->setObjectId("imageSeries", s_IMAGE_SERIES_ID);
    mesher50->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    mesher80->setObjectId("imageSeries", s_IMAGE_SERIES_ID);
    mesher80->setObjectId("modelSeries", s_MODEL_SERIES_ID);
    modelSeriesAdaptor->setObjectId("model", s_MODEL_SERIES_ID);

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    auto worker         = core::thread::Worker::New();
    auto workerRegistry = services::registry::ActiveWorkers::getDefault();
    workerRegistry->addWorker("Tuto09", worker);

    auto proxy = services::registry::Proxy::getDefault();

    std::function<void(data::Object::sptr)>  recSelectedFct =
        [ = ] (data::Object::sptr rec)
        {
            m_appManager->addObject(rec, s_RECONSTRUCTION_ID);
        };
    m_slotRecSelected = core::com::newSlot(recSelectedFct);
    m_slotRecSelected->setWorker(worker);
    proxy->connect(s_REC_SELECTED_CHANNEL, m_slotRecSelected);

    std::function<void()>  emptySelectionFct =
        [ = ] ()
        {
            m_appManager->removeObject(nullptr, s_RECONSTRUCTION_ID);
        };
    m_slotEmptySelection = core::com::newSlot(emptySelectionFct);
    m_slotEmptySelection->setWorker(worker);
    proxy->connect(s_EMPTY_SELECTION_CHANNEL, m_slotEmptySelection);

    services::helper::ProxyConnections jobCnt;
    jobCnt.addSignalConnection(imageSeriesReader->getID(), "jobCreated");
    jobCnt.addSignalConnection(modelSeriesWriter->getID(), "jobCreated");
    jobCnt.addSlotConnection(progressBar->getID(), "showJob");
    m_appManager->addProxyConnection(jobCnt);

    services::helper::ProxyConnections showScanCnt;
    showScanCnt.addSignalConnection(showScanEditor->getID(), "toggled");
    showScanCnt.addSlotConnection(sliceListEditor->getID(), "setEnabled");
    showScanCnt.addSlotConnection(imageAdaptor->getID(), "showSlice");
    m_appManager->addProxyConnection(showScanCnt);

    services::helper::ProxyConnections snapCnt;
    snapCnt.addSignalConnection(snapshotEditor->getID(), "snapped");
    snapCnt.addSlotConnection(snapshotAdaptor->getID(), "snap");
    m_appManager->addProxyConnection(snapCnt);

    services::helper::ProxyConnections sliceListCnt;
    sliceListCnt.addSignalConnection(sliceListEditor->getID(), "selected");
    sliceListCnt.addSlotConnection(imageAdaptor->getID(), "updateSliceMode");
    m_appManager->addProxyConnection(sliceListCnt);

    services::helper::ProxyConnections recSelectedCnt(s_REC_SELECTED_CHANNEL);
    recSelectedCnt.addSignalConnection(listOrganEditor->getID(),  "reconstructionSelected");
    m_appManager->addProxyConnection(recSelectedCnt);
    services::helper::ProxyConnections emptySelectionCnt(s_EMPTY_SELECTION_CHANNEL);
    emptySelectionCnt.addSignalConnection(listOrganEditor->getID(),  "emptiedSelection");
    m_appManager->addProxyConnection(emptySelectionCnt);

    services::helper::ProxyConnections mesherOnCnt;
    mesherOnCnt.addSignalConnection(mesher50->getID(), "started");
    mesherOnCnt.addSignalConnection(mesher80->getID(), "started");
    mesherOnCnt.addSlotConnection(actionCreateMesh50->getID(), "setExecutable");
    mesherOnCnt.addSlotConnection(actionCreateMesh80->getID(), "setExecutable");
    m_appManager->addProxyConnection(mesherOnCnt);

    services::helper::ProxyConnections mesherOffCnt;
    mesherOffCnt.addSignalConnection(mesher50->getID(), "stopped");
    mesherOffCnt.addSignalConnection(mesher80->getID(), "stopped");
    mesherOffCnt.addSlotConnection(actionCreateMesh50->getID(), "setInexecutable");
    mesherOffCnt.addSlotConnection(actionCreateMesh80->getID(), "setInexecutable");
    m_appManager->addProxyConnection(mesherOffCnt);

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
    auto proxy = services::registry::Proxy::getDefault();

    proxy->disconnect(s_REC_SELECTED_CHANNEL, m_slotRecSelected);
    proxy->disconnect(s_EMPTY_SELECTION_CHANNEL, m_slotEmptySelection);

    auto workerRegistry = services::registry::ActiveWorkers::getDefault();
    auto worker         = workerRegistry->getWorker("Tuto09");
    worker->stop();

    m_appManager->destroy();
    m_appManager.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto03MesherWithGenericSceneCpp
