/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto08GenericSceneCpp/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto08GenericSceneCpp
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto08GenericSceneCpp::Plugin");

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
    auto frameSrv          = m_appManager->addService("::gui::frame::SDefaultFrame", true, false);
    auto menuBar           = m_appManager->addService("::gui::aspect::SDefaultMenuBar", "menuBar", true, false);
    auto menu              = m_appManager->addService("::gui::aspect::SDefaultMenu", "menuFile", true, false);
    auto actionOpenImage   = m_appManager->addService("::gui::action::SStarter", "actionOpenImage", true, false);
    auto actionOpenMesh    = m_appManager->addService("::gui::action::SStarter", "actionOpenMesh", true, false);
    auto actionOpenTexture = m_appManager->addService("::gui::action::SStarter", "actionOpenTexture", true, false);
    auto actionQuit        = m_appManager->addService("::gui::action::SQuit", "actionQuit", true, false);
    auto mainView          = m_appManager->addService("::gui::view::SDefaultView", "mainView", true, false);
    auto editorsView       = m_appManager->addService("::gui::view::SDefaultView", "editorsView", true, false);
    auto snapshotEditor    = m_appManager->addService("::uiVisuQt::SnapshotEditor", "snapshotEditor", true, false);
    auto sliceListEditor   = m_appManager->addService("::guiQt::editor::SSelectionMenuButton", "sliceListEditor",
                                                      true, false);
    auto showScanEditor    = m_appManager->addService("::guiQt::editor::SSignalButton", "showScanEditor", true, false);
    auto sliderIndexEditor = m_appManager->addService("::uiImageQt::SliceIndexPositionEditor", "sliderIndexEditor",
                                                      true, false);
    auto imageReaderSrv   = m_appManager->addService("::uiIO::editor::SIOSelector", "imageReader", true, false);
    auto meshReaderSrv    = m_appManager->addService("::uiIO::editor::SIOSelector", "meshReader", true, false);
    auto textureReaderSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "textureReader", true, false);
    auto renderSrv        = m_appManager->addService("::fwRenderVTK::SRender", "genericScene", true, false);
    auto imageAdaptor     = m_appManager->addService("::visuVTKAdaptor::SNegatoMPR", "imageAdaptor", true, false);
    auto meshAdaptor      = m_appManager->addService("::visuVTKAdaptor::SMesh", "meshAdaptor", true, false);
    auto textureAdaptor   = m_appManager->addService("::visuVTKAdaptor::STexture", "textureAdaptor", true, false);
    auto snapshotAdaptor  = m_appManager->addService("::visuVTKAdaptor::SSnapshot", "snapshotAdaptor", true, false);
    auto progressBar      = m_appManager->addService("::gui::editor::SJobBar", true, false);

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto08GenericSceneCpp");
    frameConfig.put("gui.frame.icon", "Tuto08GenericSceneCpp-0.1/tuto.ico");
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

    imageReaderSrv->registerObject(s_IMAGE_ID, "data", ::fwServices::IService::AccessType::OUTPUT, false, true);
    meshReaderSrv->registerObject(s_MESH_ID, "data", ::fwServices::IService::AccessType::OUTPUT, false, true);
    textureReaderSrv->registerObject(s_TEXTURE_ID, "data", ::fwServices::IService::AccessType::OUTPUT, false, true);
    sliderIndexEditor->setObjectId("image", s_IMAGE_ID);
    imageAdaptor->setObjectId("image", s_IMAGE_ID );
    meshAdaptor->setObjectId("mesh", s_MESH_ID);
    textureAdaptor->setObjectId("texture", s_TEXTURE_ID);

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    ::fwServices::helper::ProxyConnections jobCnt;
    jobCnt.addSignalConnection(meshReaderSrv->getID(), "jobCreated");
    jobCnt.addSignalConnection(imageReaderSrv->getID(), "jobCreated");
    jobCnt.addSignalConnection(textureReaderSrv->getID(), "jobCreated");
    jobCnt.addSlotConnection(progressBar->getID(), "showJob");
    m_appManager->addProxyConnection(jobCnt);

    ::fwServices::helper::ProxyConnections showScanCnt;
    showScanCnt.addSignalConnection(showScanEditor->getID(), "toggled");
    showScanCnt.addSlotConnection(sliceListEditor->getID(), "setEnabled");
    showScanCnt.addSlotConnection(imageAdaptor->getID(), "showSlice");
    m_appManager->addProxyConnection(showScanCnt);

    ::fwServices::helper::ProxyConnections snapCnt;
    snapCnt.addSignalConnection(snapshotEditor->getID(), "snapped");
    snapCnt.addSlotConnection(snapshotAdaptor->getID(), "snap");
    m_appManager->addProxyConnection(snapCnt);

    ::fwServices::helper::ProxyConnections sliceListCnt;
    sliceListCnt.addSignalConnection(sliceListEditor->getID(), "selected");
    sliceListCnt.addSlotConnection(imageAdaptor->getID(), "updateSliceMode");
    m_appManager->addProxyConnection(sliceListCnt);

    ::fwServices::helper::ProxyConnections textureCnt;
    textureCnt.addSignalConnection(meshAdaptor->getID(), "textureApplied");
    textureCnt.addSlotConnection(textureAdaptor->getID(), "applyTexture");
    m_appManager->addProxyConnection(textureCnt);

    ::fwServices::helper::ProxyConnections textureUpdatedCnt;
    textureUpdatedCnt.addSignalConnection(textureAdaptor->getID(), "started");
    textureUpdatedCnt.addSlotConnection(meshAdaptor->getID(), "update");
    m_appManager->addProxyConnection(textureUpdatedCnt);

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

} // namespace Tuto08GenericSceneCpp
