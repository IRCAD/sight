/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD,2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto09MesherWithGenericSceneCtrl/Plugin.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <boost/foreach.hpp>

namespace Tuto09MesherWithGenericSceneCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar(
    "::Tuto09MesherWithGenericSceneCtrl::Plugin");

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
    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/
    // GUI
    auto frameSrv         = ::fwServices::add("::gui::frame::SDefaultFrame");
    auto menuBar          = ::fwServices::add("::gui::aspect::SDefaultMenuBar", "menuBar");
    auto menuFile         = ::fwServices::add("::gui::aspect::SDefaultMenu", "menuFile");
    auto menuMesher       = ::fwServices::add("::gui::aspect::SDefaultMenu", "menuMesher");
    auto mainView         = ::fwServices::add("::gui::view::SDefaultView", "mainView");
    auto multiViewOrgans  = ::fwServices::add("::gui::view::SDefaultView", "multiViewOrgans");
    auto sceneEditorsView = ::fwServices::add("::gui::view::SDefaultView", "scenesceneEditorsView");

    // actions
    auto actionOpenImage       = ::fwServices::add("::gui::action::SStarter", "actionOpenImage");
    auto actionSaveModelSeries = ::fwServices::add("::gui::action::SStarter", "actionSaveModelSeries");
    auto actionCreateMesh50    = ::fwServices::add("::gui::action::SStarter", "actionCreateMesh50");
    auto actionCreateMesh80    = ::fwServices::add("::gui::action::SStarter", "actionCreateMesh80");
    auto actionQuit            = ::fwServices::add("::gui::action::SQuit", "actionQuit");

    //readers/writers
    auto imageSeriesReader = ::fwServices::add("::uiIO::editor::SIOSelector", "imageSeriesReader");
    auto modelSeriesWriter = ::fwServices::add("::uiIO::editor::SIOSelector", "modelSeriesWriter");

    // extrator/converter
    auto medicaImageConverter = ::fwServices::add("::ctrlSelection::MedicalImageSrv", "medicaImageConverter");

    //editors
    auto snapshotEditor       = ::fwServices::add("::uiVisuQt::SnapshotEditor", "snapshotEditor");
    auto sliceListEditor      = ::fwServices::add("::guiQt::editor::SSelectionMenuButton", "sliceListEditor");
    auto showScanEditor       = ::fwServices::add("::guiQt::editor::SSignalButton", "showScanEditor");
    auto sliderIndexEditor    = ::fwServices::add("::uiImageQt::SliceIndexPositionEditor", "sliderIndexEditor");
    auto listOrganEditor      = ::fwServices::add("::uiMedDataQt::editor::SModelSeriesList", "listOrganEditor");
    auto organMaterialEditor  = ::fwServices::add("::uiReconstructionQt::OrganMaterialEditor", "organMaterialEditor");
    auto representationEditor = ::fwServices::add("::uiReconstructionQt::RepresentationEditor", "representationEditor");
    auto progressBar          = ::fwServices::add("::gui::editor::SJobBar");

    // meshers
    auto mesher50 = ::fwServices::add("::opVTKMesh::SVTKMesher", "mesher50");
    auto mesher80 = ::fwServices::add("::opVTKMesh::SVTKMesher", "mesher80");

    // generic scene
    auto renderSrv          = ::fwServices::add("::fwRenderVTK::SRender", "genericScene");
    auto imageAdaptor       = ::fwServices::add("::visuVTKAdaptor::SNegatoMPR", "imageAdaptor");
    auto modelSeriesAdaptor = ::fwServices::add("::visuVTKAdaptor::SModelSeries", "modelSeriesAdaptor");
    auto snapshotAdaptor    = ::fwServices::add("::visuVTKAdaptor::SSnapshot", "snapshotAdaptor");

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto09MesherWithGenericSceneCtrl");
    frameConfig.put("gui.frame.icon", "Tuto09MesherWithGenericSceneCtrl-0.1/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.menuBar.<xmlattr>.start", "yes");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameConfig.put("registry.view.<xmlattr>.start", "yes");
    frameSrv->setConfiguration( frameConfig );
    frameSrv->configure();

    // create the menuBar configuration
    ::fwServices::IService::ConfigType menuBarConfig;
    ::fwServices::IService::ConfigType menuBarMenu1Config;
    menuBarMenu1Config.put("<xmlattr>.name", "File");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu1Config);
    ::fwServices::IService::ConfigType menuBarMenu2Config;
    menuBarMenu2Config.put("<xmlattr>.name", "Mesher");
    menuBarConfig.add_child("gui.layout.menu", menuBarMenu2Config);
    ::fwServices::IService::ConfigType menuBarMenu1ConfigReg;
    menuBarMenu1ConfigReg.put("<xmlattr>.sid", "menuFile");
    menuBarMenu1ConfigReg.put("<xmlattr>.start", "yes");
    menuBarConfig.add_child("registry.menu", menuBarMenu1ConfigReg);
    ::fwServices::IService::ConfigType menuBarMenu2ConfigReg;
    menuBarMenu2ConfigReg.put("<xmlattr>.sid", "menuMesher");
    menuBarMenu2ConfigReg.put("<xmlattr>.start", "yes");
    menuBarConfig.add_child("registry.menu", menuBarMenu2ConfigReg);
    menuBar->setConfiguration(menuBarConfig);
    menuBar->configure();

    // menu File
    ::fwServices::IService::ConfigType menuFileConfig;
    ::fwServices::IService::ConfigType menuFileItem1;
    menuFileItem1.put("<xmlattr>.name", "Open image");
    menuFileItem1.put("<xmlattr>.shortcut", "Ctrl+O");
    ::fwServices::IService::ConfigType menuFileItem2;
    menuFileItem2.put("<xmlattr>.name", "Save meshes");
    menuFileItem2.put("<xmlattr>.shortcut", "Ctrl+S");
    ::fwServices::IService::ConfigType menuFileItem3;
    menuFileItem3.put("<xmlattr>.name", "Quit");
    menuFileItem3.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuFileItem3.put("<xmlattr>.specialAction", "QUIT");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem1);
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem2);
    menuFileConfig.add("gui.layout", "separator");
    menuFileConfig.add_child("gui.layout.menuItem", menuFileItem3);

    ::fwServices::IService::ConfigType menuFileItem1Reg;
    menuFileItem1Reg.put("<xmlattr>.sid", "actionOpenImage");
    menuFileItem1Reg.put("<xmlattr>.start", "yes");
    ::fwServices::IService::ConfigType menuFileItem2Reg;
    menuFileItem2Reg.put("<xmlattr>.sid", "actionSaveModelSeries");
    ::fwServices::IService::ConfigType menuFileItem3Reg;
    menuFileItem3Reg.put("<xmlattr>.sid", "actionQuit");
    menuFileItem3Reg.put("<xmlattr>.start", "yes");
    menuFileConfig.add_child("registry.menuItem", menuFileItem1Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem2Reg);
    menuFileConfig.add_child("registry.menuItem", menuFileItem3Reg);
    menuFile->setConfiguration(menuFileConfig);
    menuFile->configure();

    // menu mesher
    ::fwServices::IService::ConfigType menuMesherConfig;
    ::fwServices::IService::ConfigType menuMesherItem1;
    menuMesherItem1.put("<xmlattr>.name", "Create Mesh 50");
    ::fwServices::IService::ConfigType menuMesherItem2;
    menuMesherItem2.put("<xmlattr>.name", "Create Mesh 80");
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem1);
    menuMesherConfig.add_child("gui.layout.menuItem", menuMesherItem2);

    ::fwServices::IService::ConfigType menuMesherItem1Reg;
    menuMesherItem1Reg.put("<xmlattr>.sid", "actionCreateMesh50");
    ::fwServices::IService::ConfigType menuMesherItem2Reg;
    menuMesherItem2Reg.put("<xmlattr>.sid", "actionCreateMesh80");
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem1Reg);
    menuMesherConfig.add_child("registry.menuItem", menuMesherItem2Reg);
    menuMesher->setConfiguration(menuMesherConfig);
    menuMesher->configure();

    // mainview
    ::fwServices::IService::ConfigType mainViewConfig;
    ::fwServices::IService::ConfigType mainViewLayoutConfig;
    mainViewLayoutConfig.put("<xmlattr>.type", "::fwGui::CardinalLayoutManager");
    ::fwServices::IService::ConfigType mainView1Config;
    mainView1Config.put("<xmlattr>.align", "center");
    ::fwServices::IService::ConfigType mainView2Config;
    mainView2Config.put("<xmlattr>.align", "right");
    mainView2Config.put("<xmlattr>.minWidth", "300");
    ::fwServices::IService::ConfigType mainView3Config;
    mainView3Config.put("<xmlattr>.align", "bottom");
    mainView3Config.put("<xmlattr>.minHeight", "30");
    mainViewLayoutConfig.add_child("view", mainView1Config);
    mainViewLayoutConfig.add_child("view", mainView2Config);
    mainViewLayoutConfig.add_child("view", mainView3Config);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    ::fwServices::IService::ConfigType mainView1ConfigReg;
    mainView1ConfigReg.put("<xmlattr>.sid", "genericScene");
    mainView1ConfigReg.put("<xmlattr>.start", "yes");
    mainViewConfig.add_child("registry.view", mainView1ConfigReg);
    ::fwServices::IService::ConfigType mainView2ConfigReg;
    mainView2ConfigReg.put("<xmlattr>.sid", "multiViewOrgans");
    mainView2ConfigReg.put("<xmlattr>.start", "yes");
    mainViewConfig.add_child("registry.view", mainView2ConfigReg);
    ::fwServices::IService::ConfigType mainView3ConfigReg;
    mainView3ConfigReg.put("<xmlattr>.sid", "scenesceneEditorsView");
    mainView3ConfigReg.put("<xmlattr>.start", "yes");
    mainViewConfig.add_child("registry.view", mainView3ConfigReg);
    mainView->setConfiguration(mainViewConfig);
    mainView->configure();

    // multiViewOrgans
    ::fwServices::IService::ConfigType multiViewOrgansConfig;
    ::fwServices::IService::ConfigType multiViewOrgansLayoutConfig;
    multiViewOrgansLayoutConfig.put("<xmlattr>.type", "::fwGui::ToolboxLayoutManager");
    ::fwServices::IService::ConfigType multiViewOrgans1Config;
    multiViewOrgans1Config.put("<xmlattr>.caption", "Organs");
    multiViewOrgans1Config.put("<xmlattr>.expanded", true);
    ::fwServices::IService::ConfigType multiViewOrgans2Config;
    multiViewOrgans2Config.put("<xmlattr>.caption", "Material");
    multiViewOrgans2Config.put("<xmlattr>.expanded", true);
    ::fwServices::IService::ConfigType multiViewOrgans3Config;
    multiViewOrgans3Config.put("<xmlattr>.caption", "Representation");
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans1Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans2Config);
    multiViewOrgansLayoutConfig.add_child("view", multiViewOrgans3Config);
    multiViewOrgansConfig.add_child("gui.layout", multiViewOrgansLayoutConfig);
    ::fwServices::IService::ConfigType multiViewOrgans1ConfigReg;
    multiViewOrgans1ConfigReg.put("<xmlattr>.sid", "listOrganEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans1ConfigReg);
    ::fwServices::IService::ConfigType multiViewOrgans2ConfigReg;
    multiViewOrgans2ConfigReg.put("<xmlattr>.sid", "organMaterialEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans2ConfigReg);
    ::fwServices::IService::ConfigType multiViewOrgans3ConfigReg;
    multiViewOrgans3ConfigReg.put("<xmlattr>.sid", "representationEditor");
    multiViewOrgansConfig.add_child("registry.view", multiViewOrgans3ConfigReg);
    multiViewOrgans->setConfiguration(multiViewOrgansConfig);
    multiViewOrgans->configure();

    // sceneEditorsView
    ::fwServices::IService::ConfigType sceneEditorsViewConfig;
    ::fwServices::IService::ConfigType sceneEditorsViewLayoutConfig;
    sceneEditorsViewLayoutConfig.put("<xmlattr>.type", "::fwGui::LineLayoutManager");
    ::fwServices::IService::ConfigType editorsOrientation;
    editorsOrientation.put("<xmlattr>.value", "horizontal");
    sceneEditorsViewLayoutConfig.add_child("orientation", editorsOrientation);
    ::fwServices::IService::ConfigType sceneEditorsView1;
    sceneEditorsView1.put("<xmlattr>.proportion", "0");
    sceneEditorsView1.put("<xmlattr>.minWidth", "30");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView1);
    ::fwServices::IService::ConfigType sceneEditorsView2;
    sceneEditorsView2.put("<xmlattr>.proportion", "0");
    sceneEditorsView2.put("<xmlattr>.minWidth", "50");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView2);
    ::fwServices::IService::ConfigType sceneEditorsView3;
    sceneEditorsView3.put("<xmlattr>.proportion", "1");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView3);
    ::fwServices::IService::ConfigType sceneEditorsView4;
    sceneEditorsView4.put("<xmlattr>.proportion", "0");
    sceneEditorsView4.put("<xmlattr>.minWidth", "30");
    sceneEditorsViewLayoutConfig.add_child("view", sceneEditorsView4);
    menuFileConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    sceneEditorsViewConfig.add_child("gui.layout", sceneEditorsViewLayoutConfig);
    ::fwServices::IService::ConfigType sceneEditorsView1Reg;
    sceneEditorsView1Reg.put("<xmlattr>.sid", "sliceListEditor");
    sceneEditorsView1Reg.put("<xmlattr>.start", "yes");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView1Reg);
    ::fwServices::IService::ConfigType sceneEditorsView2Reg;
    sceneEditorsView2Reg.put("<xmlattr>.sid", "showScanEditor");
    sceneEditorsView2Reg.put("<xmlattr>.start", "yes");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView2Reg);
    ::fwServices::IService::ConfigType sceneEditorsView3Reg;
    sceneEditorsView3Reg.put("<xmlattr>.sid", "sliderIndexEditor");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView3Reg);
    ::fwServices::IService::ConfigType sceneEditorsView4Reg;
    sceneEditorsView4Reg.put("<xmlattr>.sid", "snapshotEditor");
    sceneEditorsView4Reg.put("<xmlattr>.start", "yes");
    sceneEditorsViewConfig.add_child("registry.view", sceneEditorsView4Reg);
    sceneEditorsView->setConfiguration(sceneEditorsViewConfig);
    sceneEditorsView->configure();

    /* **************************************************************************************
    *              actions configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType actionOpenImageConfig;
    actionOpenImageConfig.add("start.<xmlattr>.uid", "imageSeriesReader");
    actionOpenImage->setConfiguration(actionOpenImageConfig);
    actionOpenImage->configure();

    ::fwServices::IService::ConfigType actionSaveModelSeriesConfig;
    actionSaveModelSeriesConfig.add("start.<xmlattr>.uid", "meshReader");
    actionSaveModelSeries->setConfiguration(actionSaveModelSeriesConfig);
    actionSaveModelSeries->configure();

    ::fwServices::IService::ConfigType actionCreateMesh50Config;
    actionCreateMesh50Config.add("start.<xmlattr>.uid", "mesher50");
    actionCreateMesh50->setConfiguration(actionCreateMesh50Config);
    actionCreateMesh50->configure();

    ::fwServices::IService::ConfigType actionCreateMesh80Config;
    actionCreateMesh80Config.add("start.<xmlattr>.uid", "mesher80");
    actionCreateMesh80->setConfiguration(actionCreateMesh80Config);
    actionCreateMesh80->configure();

    actionQuit->configure();

    /* **************************************************************************************
    *              readers/writers configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType imageSeriesReaderConfig;
    imageSeriesReaderConfig.put("type.<xmlattr>.mode", "reader");
    imageSeriesReaderConfig.put("type.<xmlattr>.class", "::fwMedData::ImageSeries");
    imageSeriesReader->setConfiguration(imageSeriesReaderConfig);
    imageSeriesReader->configure();

    ::fwServices::IService::ConfigType modelSeriesWriterConfig;
    modelSeriesWriterConfig.put("type.<xmlattr>.mode", "writer");
    modelSeriesWriter->setConfiguration(modelSeriesWriterConfig);
    modelSeriesWriter->configure();

    /* **************************************************************************************
    *              editors configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType sliceListEditorConfig;
    sliceListEditorConfig.add("toolTip", "Manage slice visibility");
    sliceListEditorConfig.add("selected", "3");
    ::fwServices::IService::ConfigType sliceListEditorItem1Config;
    sliceListEditorItem1Config.put("<xmlattr>.text", "One slice");
    sliceListEditorItem1Config.put("<xmlattr>.value", "1");
    sliceListEditorConfig.add_child("items.item", sliceListEditorItem1Config);
    ::fwServices::IService::ConfigType sliceListEditorItem2Config;
    sliceListEditorItem2Config.put("<xmlattr>.text", "Three slices");
    sliceListEditorItem2Config.put("<xmlattr>.value", "3");
    sliceListEditorConfig.add_child("items.item", sliceListEditorItem2Config);
    sliceListEditor->setConfiguration(sliceListEditorConfig);
    sliceListEditor->configure();

    ::fwServices::IService::ConfigType showScanEditorConfig;
    showScanEditorConfig.add("config.checkable", true);
    showScanEditorConfig.add("config.icon", "media-0.1/icons/sliceHide.png");
    showScanEditorConfig.add("config.icon2", "media-0.1/icons/sliceShow.png");
    showScanEditorConfig.add("config.iconWidth", "40");
    showScanEditorConfig.add("config.iconHeight", "16");
    showScanEditorConfig.add("config.checked", true);
    showScanEditor->setConfiguration(showScanEditorConfig);
    showScanEditor->configure();

    ::fwServices::IService::ConfigType sliderIndexEditorConfig;
    sliderIndexEditorConfig.put("sliceIndex", "axial");
    sliderIndexEditor->setConfiguration(sliderIndexEditorConfig);
    sliderIndexEditor->configure();

    ::fwServices::IService::ConfigType listOrganEditorConfig;
    listOrganEditorConfig.add("columns.organ_name", "@organ_name");
    listOrganEditor->setConfiguration(listOrganEditorConfig);
    listOrganEditor->configure();

    organMaterialEditor->configure();
    representationEditor->configure();
    snapshotEditor->configure();

    /* **************************************************************************************
    *              genericScene configuration
    ****************************************************************************************/

    // create and register the render service
    ::fwServices::IService::ConfigType renderConfig;
    ::fwServices::IService::ConfigType pickerConfig;
    pickerConfig.add("<xmlattr>.id", "picker");
    pickerConfig.add("<xmlattr>.vtkclass", "fwVtkCellPicker");
    renderConfig.add_child("scene.picker", pickerConfig);
    renderConfig.add("scene.renderer.<xmlattr>.id", "default");
    ::fwServices::IService::ConfigType adpt1Config;
    adpt1Config.put("<xmlattr>.uid", "modelSeriesAdaptor");
    renderConfig.add_child("scene.adaptor", adpt1Config);
    ::fwServices::IService::ConfigType adpt2Config;
    adpt2Config.put("<xmlattr>.uid", "imageAdaptor");
    renderConfig.add_child("scene.adaptor", adpt2Config);
    ::fwServices::IService::ConfigType adpt3Config;
    adpt3Config.put("<xmlattr>.uid", "snapshotAdaptor");
    renderConfig.add_child("scene.adaptor", adpt3Config);
    renderSrv->setConfiguration(renderConfig);
    renderSrv->configure();

    ::fwServices::IService::ConfigType imageAdaptorConfig;
    imageAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    imageAdaptorConfig.add("config.<xmlattr>.picker", "picker");
    imageAdaptorConfig.add("config.<xmlattr>.mode", "3d");
    imageAdaptorConfig.add("config.<xmlattr>.slice", "3");
    imageAdaptorConfig.add("config.<xmlattr>.sliceIndex", "axial");
    imageAdaptor->setConfiguration(imageAdaptorConfig);
    imageAdaptor->configure();

    ::fwServices::IService::ConfigType modelSeriesAdaptorConfig;
    modelSeriesAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    modelSeriesAdaptorConfig.add("config.<xmlattr>.picker", "");
    modelSeriesAdaptor->setConfiguration(modelSeriesAdaptorConfig);
    modelSeriesAdaptor->configure();

    ::fwServices::IService::ConfigType snapshotAdaptorConfig;
    snapshotAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    snapshotAdaptor->setConfiguration(snapshotAdaptorConfig);
    snapshotAdaptor->configure();

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    auto worker         = ::fwThread::Worker::New();
    auto workerRegistry = ::fwServices::registry::ActiveWorkers::getDefault();
    workerRegistry->addWorker("Tuto09", worker);

    std::function<void()>  imageSeriesFct =
        [ = ] ()
        {
            SLM_ASSERT("imageSeriesReader is expired !", imageSeriesReader);
            auto imageSeries =
                imageSeriesReader->getOutput< ::fwMedData::ImageSeries >("data");
            if (imageSeries)
            {
                ::fwData::Image::sptr image = imageSeries->getImage();

                if(::fwDataTools::fieldHelper::MedicalImageHelpers::
                   checkImageValidity(image))
                {
                    ::fwDataTools::helper::Image helper( image );

                    helper.createLandmarks();
                    helper.createTransferFunctionPool();
                    helper.createImageSliceIndex();

                    sliceListEditor->signal("selected")->connect(imageAdaptor->slot(
                                                                     "updateSliceMode"));
                    showScanEditor->signal("toggled")->connect(sliceListEditor->slot(
                                                                   "setEnabled"));
                    showScanEditor->signal("toggled")->connect(imageAdaptor->slot(
                                                                   "showSlice"));

                    if (::fwServices::OSR::isRegistered("image",
                                                        ::fwServices::IService::
                                                        AccessType::INOUT,
                                                        imageAdaptor))
                    {
                        imageAdaptor->stop();
                        ::fwServices::OSR::unregisterService("image",
                                                             ::fwServices::IService::AccessType::INOUT,
                                                             imageAdaptor);
                        sliderIndexEditor->stop();
                        ::fwServices::OSR::unregisterService("image",
                                                             ::fwServices::IService::AccessType::INOUT,
                                                             sliderIndexEditor);
                        mesher50->stop();
                        ::fwServices::OSR::unregisterService("imageSeries",
                                                             ::fwServices::IService::AccessType::INPUT,
                                                             mesher50);
                        mesher80->stop();
                        ::fwServices::OSR::unregisterService("imageSeries",
                                                             ::fwServices::IService::AccessType::INPUT,
                                                             mesher80);
                    }
                    imageAdaptor->registerInOut(image, "image", true);
                    sliderIndexEditor->registerInOut(image, "image", true);
                    mesher50->registerInput(imageSeries, "imageSeries");
                    mesher80->registerInput(imageSeries, "imageSeries");
                    imageAdaptor->start();
                    sliderIndexEditor->start();
                    mesher50->start();
                    mesher80->start();
                    actionCreateMesh50->start();
                    actionCreateMesh80->start();

                    m_startedService.emplace_back(imageAdaptor);
                    m_startedService.emplace_back(sliderIndexEditor);
                    m_startedService.emplace_back(mesher50);
                    m_startedService.emplace_back(mesher80);
                    m_startedService.emplace_back(actionCreateMesh50);
                    m_startedService.emplace_back(actionCreateMesh80);
                }
            }
        };
    auto imageReaderSlot = ::fwCom::newSlot(imageSeriesFct);
    imageReaderSlot->setWorker(worker);
    m_slots.push_back(imageReaderSlot);

    std::function<void(::fwData::Object::sptr)>  recSelectedFct =
        [ = ] (::fwData::Object::sptr rec)
        {
            if (organMaterialEditor->isStarted())
            {
                organMaterialEditor->stop();
                ::fwServices::OSR::unregisterService("reconstruction",
                                                     ::fwServices::IService::
                                                     AccessType::INOUT,
                                                     organMaterialEditor);

                representationEditor->stop();
                ::fwServices::OSR::unregisterService("reconstruction",
                                                     ::fwServices::IService::
                                                     AccessType::INOUT,
                                                     representationEditor);
            }
            organMaterialEditor->registerInOut(rec, "reconstruction", true);
            representationEditor->registerInOut(rec, "reconstruction", true);
            organMaterialEditor->start();
            representationEditor->start();
            m_startedService.emplace_back(organMaterialEditor);
            m_startedService.emplace_back(representationEditor);
        };
    auto recSelectedSlot = ::fwCom::newSlot(recSelectedFct);
    recSelectedSlot->setWorker(worker);
    m_slots.push_back(recSelectedSlot);

    auto modelSeriesFct =
        [ = ] (::fwServices::IService::sptr mesher)
        {
            ::fwMedData::ModelSeries::sptr model = mesher->getOutput< ::fwMedData::ModelSeries >(
                "modelSeries");
            if (model)
            {
                if (modelSeriesAdaptor->isStarted())
                {
                    modelSeriesAdaptor->stop();
                    ::fwServices::OSR::unregisterService("modelSeries",
                                                         ::fwServices::IService::
                                                         AccessType::INPUT,
                                                         modelSeriesAdaptor);
                    listOrganEditor->stop();
                    ::fwServices::OSR::unregisterService("modelSeries",
                                                         ::fwServices::IService::
                                                         AccessType::INOUT,
                                                         listOrganEditor);
                }
                modelSeriesAdaptor->registerInput(model, "model", true);
                listOrganEditor->registerInOut(model, "modelSeries", true);
                modelSeriesAdaptor->start();
                listOrganEditor->start();
                m_startedService.emplace_back(modelSeriesAdaptor);
                m_startedService.emplace_back(listOrganEditor);

                listOrganEditor->signal("reconstructionSelected")->connect(recSelectedSlot);
            }
        };

    std::function<void()> mesher50Fct = std::bind(modelSeriesFct, mesher50);
    auto mesher50Slot                 = ::fwCom::newSlot(mesher50Fct);
    mesher50Slot->setWorker(worker);
    m_slots.push_back(mesher50Slot);
    std::function<void()> mesher80Fct = std::bind(modelSeriesFct, mesher80);
    auto mesher80Slot                 = ::fwCom::newSlot(mesher80Fct);
    mesher80Slot->setWorker(worker);
    m_slots.push_back(mesher80Slot);

    imageSeriesReader->signal("jobCreated")->connect(progressBar->slot("showJob"));
    modelSeriesWriter->signal("jobCreated")->connect(progressBar->slot("showJob"));
    snapshotEditor->signal("snapped")->connect(snapshotAdaptor->slot("snap"));

    imageSeriesReader->signal("updated")->connect(imageReaderSlot);
    mesher50->signal("updated")->connect(mesher50Slot);
    mesher80->signal("updated")->connect(mesher80Slot);

    /* **************************************************************************************
    *              start the services
    ****************************************************************************************/

    frameSrv->start();
    snapshotAdaptor->start();
    m_startedService.emplace_back(frameSrv);
    m_startedService.emplace_back(snapshotAdaptor);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    std::vector< ::fwServices::IService::SharedFutureType > futures;

    // stop the started services
    BOOST_REVERSE_FOREACH(auto& srv, m_startedService)
    {
        futures.emplace_back(srv->stop());
    }
    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

    // unregister the services
    auto services = ::fwServices::OSR::getServices("::fwServices::IService");
    for(auto& srv : services)
    {
        ::fwServices::OSR::unregisterService(srv);
    }
    m_startedService.clear();
}

//------------------------------------------------------------------------------

} // namespace Tuto09MesherWithGenericSceneCtrl
