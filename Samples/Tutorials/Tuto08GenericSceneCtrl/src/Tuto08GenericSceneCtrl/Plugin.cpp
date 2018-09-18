/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto08GenericSceneCtrl/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto08GenericSceneCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto08GenericSceneCtrl::Plugin");

static const std::string s_IMAGE_ID   = "image";
static const std::string s_TEXTURE_ID = "texture";
static const std::string s_MESH_ID    = "mesh";

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
    m_appManager = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager );
    m_appManager->create();

    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/
    auto frameSrv          = m_appManager->registerService("::gui::frame::SDefaultFrame", "", true);
    auto menuBar           = m_appManager->registerService("::gui::aspect::SDefaultMenuBar", "menuBar", true);
    auto menu              = m_appManager->registerService("::gui::aspect::SDefaultMenu", "menuFile", true);
    auto actionOpenImage   = m_appManager->registerService("::gui::action::SStarter", "actionOpenImage", true);
    auto actionOpenMesh    = m_appManager->registerService("::gui::action::SStarter", "actionOpenMesh", true);
    auto actionOpenTexture = m_appManager->registerService("::gui::action::SStarter", "actionOpenTexture", true);
    auto actionQuit        = m_appManager->registerService("::gui::action::SQuit", "actionQuit", true);
    auto mainView          = m_appManager->registerService("::gui::view::SDefaultView", "mainView", true);
    auto editorsView       = m_appManager->registerService("::gui::view::SDefaultView", "editorsView", true);
    auto snapshotEditor    = m_appManager->registerService("::uiVisuQt::SnapshotEditor", "snapshotEditor", true);
    auto sliceListEditor   = m_appManager->registerService("::guiQt::editor::SSelectionMenuButton", "sliceListEditor",
                                                           true);
    auto showScanEditor    = m_appManager->registerService("::guiQt::editor::SSignalButton", "showScanEditor", true);
    auto sliderIndexEditor = m_appManager->registerService("::uiImageQt::SliceIndexPositionEditor", "sliderIndexEditor",
                                                           true);
    auto imageReaderSrv   = m_appManager->registerService("::uiIO::editor::SIOSelector", "imageReader", true);
    auto meshReaderSrv    = m_appManager->registerService("::uiIO::editor::SIOSelector", "meshReader", true);
    auto textureReaderSrv = m_appManager->registerService("::uiIO::editor::SIOSelector", "textureReader", true);
    auto renderSrv        = m_appManager->registerService("::fwRenderVTK::SRender", "genericScene", true);
    auto imageAdaptor     = m_appManager->registerService("::visuVTKAdaptor::SNegatoMPR", "imageAdaptor", true);
    auto meshAdaptor      = m_appManager->registerService("::visuVTKAdaptor::SMesh", "meshAdaptor", true);
    auto textureAdaptor   = m_appManager->registerService("::visuVTKAdaptor::STexture", "textureAdaptor", true);
    auto snapshotAdaptor  = m_appManager->registerService("::visuVTKAdaptor::SSnapshot", "snapshotAdaptor", true);
    auto progressBar      = m_appManager->registerService("::gui::editor::SJobBar", "", true);

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto08GenericSceneCtrl");
    frameConfig.put("gui.frame.icon", "Tuto08GenericSceneCtrl-0.1/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameSrv->setConfiguration( frameConfig );
    frameSrv->configure();

    // create the menuBar configuration
    ::fwServices::IService::ConfigType menuBarConfig;
    menuBarConfig.put("gui.layout.menu.<xmlattr>.name", "File");
    menuBarConfig.put("registry.menu.<xmlattr>.sid", "menuFile");
    menuBar->setConfiguration(menuBarConfig);
    menuBar->configure();

    // create the menu configuration
    ::fwServices::IService::ConfigType menuConfig;
    ::fwServices::IService::ConfigType menuItem1;
    menuItem1.put("<xmlattr>.shortcut", "Ctrl+I");
    menuItem1.put("<xmlattr>.name", "Open image");
    ::fwServices::IService::ConfigType menuItem2;
    menuItem2.put("<xmlattr>.name", "Open mesh");
    menuItem2.put("<xmlattr>.shortcut", "Ctrl+M");
    ::fwServices::IService::ConfigType menuItem3;
    menuItem3.put("<xmlattr>.name", "Open texture");
    menuItem3.put("<xmlattr>.shortcut", "Ctrl+T");
    ::fwServices::IService::ConfigType menuItem4;
    menuItem4.put("<xmlattr>.name", "Quit");
    menuItem4.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuItem4.put("<xmlattr>.specialAction", "QUIT");

    menuConfig.add_child("gui.layout.menuItem", menuItem1);
    menuConfig.add_child("gui.layout.menuItem", menuItem2);
    menuConfig.add_child("gui.layout.menuItem", menuItem3);
    menuConfig.add("gui.layout", "separator");
    menuConfig.add_child("gui.layout.menuItem", menuItem4);

    ::fwServices::IService::ConfigType menuItem1Reg;
    menuItem1Reg.put("<xmlattr>.sid", "actionOpenImage");
    ::fwServices::IService::ConfigType menuItem2Reg;
    menuItem2Reg.put("<xmlattr>.sid", "actionOpenMesh");
    ::fwServices::IService::ConfigType menuItem3Reg;
    menuItem3Reg.put("<xmlattr>.sid", "actionOpenTexture");
    ::fwServices::IService::ConfigType menuItem4Reg;
    menuItem4Reg.put("<xmlattr>.sid", "actionQuit");
    menuConfig.add_child("registry.menuItem", menuItem1Reg);
    menuConfig.add_child("registry.menuItem", menuItem2Reg);
    menuConfig.add_child("registry.menuItem", menuItem3Reg);
    menuConfig.add_child("registry.menuItem", menuItem4Reg);
    menu->setConfiguration(menuConfig);
    menu->configure();

    ::fwServices::IService::ConfigType mainViewConfig;
    ::fwServices::IService::ConfigType mainViewLayoutConfig;
    mainViewLayoutConfig.put("<xmlattr>.type", "::fwGui::CardinalLayoutManager");
    ::fwServices::IService::ConfigType mainView1;
    mainView1.put("<xmlattr>.align", "center");
    ::fwServices::IService::ConfigType mainView2;
    mainView2.put("<xmlattr>.align", "bottom");
    mainView2.put("<xmlattr>.minWidth", "400");
    mainView2.put("<xmlattr>.minHeight", "30");
    mainViewLayoutConfig.add_child("view", mainView1);
    mainViewLayoutConfig.add_child("view", mainView2);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    ::fwServices::IService::ConfigType mainView1Reg;
    mainView1Reg.put("<xmlattr>.sid", "genericScene");
    ::fwServices::IService::ConfigType mainView2Reg;
    mainView2Reg.put("<xmlattr>.sid", "editorsView");
    mainViewConfig.add_child("registry.view", mainView1Reg);
    mainViewConfig.add_child("registry.view", mainView2Reg);
    mainView->setConfiguration(mainViewConfig);
    mainView->configure();

    ::fwServices::IService::ConfigType editorsViewConfig;
    ::fwServices::IService::ConfigType editorsViewLayoutConfig;
    editorsViewLayoutConfig.put("<xmlattr>.type", "::fwGui::LineLayoutManager");
    ::fwServices::IService::ConfigType editorsOrientation;
    editorsOrientation.put("<xmlattr>.value", "horizontal");
    editorsViewLayoutConfig.add_child("orientation", editorsOrientation);
    ::fwServices::IService::ConfigType editorsView1;
    editorsView1.put("<xmlattr>.proportion", "0");
    editorsView1.put("<xmlattr>.minWidth", "30");
    editorsViewLayoutConfig.add_child("view", editorsView1);
    ::fwServices::IService::ConfigType editorsView2;
    editorsView2.put("<xmlattr>.proportion", "0");
    editorsView2.put("<xmlattr>.minWidth", "50");
    editorsViewLayoutConfig.add_child("view", editorsView2);
    ::fwServices::IService::ConfigType editorsView3;
    editorsView3.put("<xmlattr>.proportion", "1");
    editorsViewLayoutConfig.add_child("view", editorsView3);
    ::fwServices::IService::ConfigType editorsView4;
    editorsView4.put("<xmlattr>.proportion", "0");
    editorsView4.put("<xmlattr>.minWidth", "30");
    editorsViewLayoutConfig.add_child("view", editorsView4);
    menuConfig.add_child("gui.layout", editorsViewLayoutConfig);
    editorsViewConfig.add_child("gui.layout", editorsViewLayoutConfig);
    ::fwServices::IService::ConfigType editorsView1Reg;
    editorsView1Reg.put("<xmlattr>.sid", "sliceListEditor");
    editorsViewConfig.add_child("registry.view", editorsView1Reg);
    ::fwServices::IService::ConfigType editorsView2Reg;
    editorsView2Reg.put("<xmlattr>.sid", "showScanEditor");
    editorsViewConfig.add_child("registry.view", editorsView2Reg);
    ::fwServices::IService::ConfigType editorsView3Reg;
    editorsView3Reg.put("<xmlattr>.sid", "sliderIndexEditor");
    editorsViewConfig.add_child("registry.view", editorsView3Reg);
    ::fwServices::IService::ConfigType editorsView4Reg;
    editorsView4Reg.put("<xmlattr>.sid", "snapshotEditor");
    editorsViewConfig.add_child("registry.view", editorsView4Reg);
    editorsView->setConfiguration(editorsViewConfig);
    editorsView->configure();

    /* **************************************************************************************
    *              actions configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType actionOpenImageConfig;
    actionOpenImageConfig.add("start.<xmlattr>.uid", "imageReader");
    actionOpenImage->setConfiguration(actionOpenImageConfig);
    actionOpenImage->configure();

    ::fwServices::IService::ConfigType actionOpenMeshConfig;
    actionOpenMeshConfig.add("start.<xmlattr>.uid", "meshReader");
    actionOpenMesh->setConfiguration(actionOpenMeshConfig);
    actionOpenMesh->configure();

    ::fwServices::IService::ConfigType actionOpenTextureConfig;
    actionOpenTextureConfig.add("start.<xmlattr>.uid", "textureReader");
    actionOpenTexture->setConfiguration(actionOpenTextureConfig);
    actionOpenTexture->configure();

    actionQuit->configure();

    /* **************************************************************************************
    *              readers configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType imageReaderConfig;
    imageReaderConfig.put("type.<xmlattr>.mode", "reader");
    imageReaderConfig.put("type.<xmlattr>.class", "::fwData::Image");
    imageReaderSrv->setConfiguration(imageReaderConfig);
    imageReaderSrv->configure();

    ::fwServices::IService::ConfigType meshReaderConfig;
    meshReaderConfig.put("type.<xmlattr>.mode", "reader");
    meshReaderConfig.put("type.<xmlattr>.class", "::fwData::Mesh");
    meshReaderSrv->setConfiguration(meshReaderConfig);
    meshReaderSrv->configure();

    ::fwServices::IService::ConfigType textureReaderConfig;
    textureReaderConfig.put("type.<xmlattr>.mode", "reader");
    textureReaderConfig.put("type.<xmlattr>.class", "::fwData::Image");
    textureReaderSrv->setConfiguration(textureReaderConfig);
    textureReaderSrv->configure();

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
    adpt1Config.put("<xmlattr>.uid", "meshAdaptor");
    renderConfig.add_child("scene.adaptor", adpt1Config);
    ::fwServices::IService::ConfigType adpt2Config;
    adpt2Config.put("<xmlattr>.uid", "textureAdaptor");
    renderConfig.add_child("scene.adaptor", adpt2Config);
    ::fwServices::IService::ConfigType adpt3Config;
    adpt3Config.put("<xmlattr>.uid", "imageAdaptor");
    renderConfig.add_child("scene.adaptor", adpt3Config);
    ::fwServices::IService::ConfigType adpt4Config;
    adpt4Config.put("<xmlattr>.uid", "snapshotAdaptor");
    renderConfig.add_child("scene.adaptor", adpt4Config);
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

    ::fwServices::IService::ConfigType meshAdaptorConfig;
    meshAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    meshAdaptorConfig.add("config.<xmlattr>.picker", "");
    meshAdaptorConfig.add("config.<xmlattr>.uvgen", "sphere");
    meshAdaptor->setConfiguration(meshAdaptorConfig);
    meshAdaptor->configure();

    ::fwServices::IService::ConfigType textureAdaptorConfig;
    textureAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    textureAdaptorConfig.add("config.<xmlattr>.filtering", "linear");
    textureAdaptorConfig.add("config.<xmlattr>.wrapping", "repeat");
    textureAdaptor->setConfiguration(textureAdaptorConfig);
    textureAdaptor->configure();

    ::fwServices::IService::ConfigType snapshotAdaptorConfig;
    snapshotAdaptorConfig.add("config.<xmlattr>.renderer", "default");
    snapshotAdaptor->setConfiguration(snapshotAdaptorConfig);
    snapshotAdaptor->configure();

    /* **************************************************************************************
    *              register inputs/inouts
    ****************************************************************************************/

    m_appManager->registerObject(sliderIndexEditor, s_IMAGE_ID, "image", ::fwServices::IService::AccessType::INOUT,
                                 true);
    m_appManager->registerObject(imageReaderSrv, s_IMAGE_ID, "data", ::fwServices::IService::AccessType::OUTPUT, true);
    m_appManager->registerObject(meshReaderSrv, s_MESH_ID, "data", ::fwServices::IService::AccessType::OUTPUT, true);
    m_appManager->registerObject(textureReaderSrv, s_TEXTURE_ID, "data", ::fwServices::IService::AccessType::OUTPUT,
                                 true);
    m_appManager->registerObject(imageAdaptor, s_IMAGE_ID, "image", ::fwServices::IService::AccessType::INOUT, true);
    m_appManager->registerObject(meshAdaptor, s_MESH_ID, "mesh", ::fwServices::IService::AccessType::INPUT, true);
    m_appManager->registerObject(textureAdaptor, s_TEXTURE_ID, "texture", ::fwServices::IService::AccessType::INOUT,
                                 true);

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    m_appManager->connectSignal("jobsChannel", meshReaderSrv, "jobCreated");
    m_appManager->connectSignal("jobsChannel", imageReaderSrv, "jobCreated");
    m_appManager->connectSignal("jobsChannel", textureReaderSrv, "jobCreated");
    m_appManager->connectSlot("jobsChannel", progressBar, "showJob");

    m_appManager->connectSignal("showScanChannel", showScanEditor, "toggled");
    m_appManager->connectSlot("showScanChannel", sliceListEditor, "setEnabled");
    m_appManager->connectSlot("showScanChannel", imageAdaptor, "showSlice");

    m_appManager->connectSignal("snapChannel", snapshotEditor, "snapped");
    m_appManager->connectSlot("snapChannel", snapshotAdaptor, "snap");

    m_appManager->connectSignal("sliceListChannel", sliceListEditor, "selected");
    m_appManager->connectSlot("sliceListChannel", imageAdaptor, "updateSliceMode");

    m_appManager->connectSignal("textureChannel", meshAdaptor, "textureApplied");
    m_appManager->connectSlot("textureChannel", textureAdaptor, "applyTexture");

    m_appManager->connectSignal("textureUpdatedChannel", textureAdaptor, "started");
    m_appManager->connectSlot("textureUpdatedChannel", meshAdaptor, "update");

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
    m_appManager->destroy();
    m_appManager.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto08GenericSceneCtrl
