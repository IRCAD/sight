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

#include "Tuto08GenericSceneCtrl/Plugin.hpp"

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <boost/foreach.hpp>

namespace Tuto08GenericSceneCtrl
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto08GenericSceneCtrl::Plugin");

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
    // create an empty image
    m_image   = ::fwData::Image::New();
    m_texture = ::fwData::Image::New();
    m_mesh    = ::fwData::Mesh::New();

    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/
    auto frameSrv          = ::fwServices::add("::gui::frame::SDefaultFrame");
    auto menuBar           = ::fwServices::add("::gui::aspect::SDefaultMenuBar", "menuBar");
    auto menu              = ::fwServices::add("::gui::aspect::SDefaultMenu", "menuFile");
    auto actionOpenImage   = ::fwServices::add("::gui::action::SStarter", "actionOpenImage");
    auto actionOpenMesh    = ::fwServices::add("::gui::action::SStarter", "actionOpenMesh");
    auto actionOpenTexture = ::fwServices::add("::gui::action::SStarter", "actionOpenTexture");
    auto actionQuit        = ::fwServices::add("::gui::action::SQuit", "actionQuit");
    auto mainView          = ::fwServices::add("::gui::view::SDefaultView", "mainView");
    auto editorsView       = ::fwServices::add("::gui::view::SDefaultView", "editorsView");
    auto snapshotEditor    = ::fwServices::add("::uiVisuQt::SnapshotEditor", "snapshotEditor");
    auto sliceListEditor   = ::fwServices::add("::guiQt::editor::SSelectionMenuButton", "sliceListEditor");
    auto showScanEditor    = ::fwServices::add("::guiQt::editor::SSignalButton", "showScanEditor");
    auto sliderIndexEditor = ::fwServices::add("::uiImageQt::SliceIndexPositionEditor", "sliderIndexEditor");
    auto imageReaderSrv    = ::fwServices::add("::uiIO::editor::SIOSelector", "imageReader");
    auto meshReaderSrv     = ::fwServices::add("::uiIO::editor::SIOSelector", "meshReader");
    auto textureReaderSrv  = ::fwServices::add("::uiIO::editor::SIOSelector", "textureReader");
    auto renderSrv         = ::fwServices::add("::fwRenderVTK::SRender", "genericScene");
    auto imageAdaptor      = ::fwServices::add("::visuVTKAdaptor::SNegatoMPR", "imageAdaptor");
    auto meshAdaptor       = ::fwServices::add("::visuVTKAdaptor::SMesh", "meshAdaptor");
    auto textureAdaptor    = ::fwServices::add("::visuVTKAdaptor::STexture", "textureAdaptor");
    auto snapshotAdaptor   = ::fwServices::add("::visuVTKAdaptor::SSnapshot", "snapshotAdaptor");
    auto progressBar       = ::fwServices::add("::gui::editor::SJobBar");

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/
    // create the frame configuration
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto08GenericSceneCtrl");
    frameConfig.put("gui.frame.icon", "Tuto08GenericSceneCtrl-0.1/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.menuBar.<xmlattr>.start", "yes");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameConfig.put("registry.view.<xmlattr>.start", "yes");
    frameSrv->setConfiguration( frameConfig );
    frameSrv->configure();

    // create the menuBar configuration
    ::fwServices::IService::ConfigType menuBarConfig;
    menuBarConfig.put("gui.layout.menu.<xmlattr>.name", "File");
    menuBarConfig.put("registry.menu.<xmlattr>.sid", "menuFile");
    menuBarConfig.put("registry.menu.<xmlattr>.start", "yes");
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

    menuConfig.add_child("gui.layout.menuItem", menuItem1);
    menuConfig.add_child("gui.layout.menuItem", menuItem2);
    menuConfig.add_child("gui.layout.menuItem", menuItem3);
    menuConfig.add("gui.layout", "separator");
    menuConfig.add_child("gui.layout.menuItem", menuItem4);

    ::fwServices::IService::ConfigType menuItem1Reg;
    menuItem1Reg.put("<xmlattr>.sid", "actionOpenImage");
    menuItem1Reg.put("<xmlattr>.start", "yes");
    ::fwServices::IService::ConfigType menuItem2Reg;
    menuItem2Reg.put("<xmlattr>.sid", "actionOpenMesh");
    menuItem2Reg.put("<xmlattr>.start", "yes");
    ::fwServices::IService::ConfigType menuItem3Reg;
    menuItem3Reg.put("<xmlattr>.sid", "actionOpenTexture");
    menuItem3Reg.put("<xmlattr>.start", "yes");
    ::fwServices::IService::ConfigType menuItem4Reg;
    menuItem4Reg.put("<xmlattr>.sid", "actionQuit");
    menuItem4Reg.put("<xmlattr>.start", "yes");
    menuConfig.add_child("registry.menuItem", menuItem1Reg);
    menuConfig.add_child("registry.menuItem", menuItem2Reg);
    menuConfig.add_child("registry.menuItem", menuItem3Reg);
    menuConfig.add_child("registry.menuItem", menuItem4Reg);

    menu->setConfiguration(menuConfig);
    menu->configure();

    actionOpenImage->configure();
    actionOpenMesh->configure();
    actionOpenTexture->configure();
    actionQuit->configure();

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
    menuConfig.add_child("gui.layout", mainViewLayoutConfig);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    ::fwServices::IService::ConfigType mainView1Reg;
    mainView1Reg.put("<xmlattr>.sid", "genericScene");
    mainView1Reg.put("<xmlattr>.start", "yes");
    ::fwServices::IService::ConfigType mainView2Reg;
    mainView2Reg.put("<xmlattr>.sid", "editorsView");
    mainView2Reg.put("<xmlattr>.start", "yes");
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
    editorsView1Reg.put("<xmlattr>.start", "yes");
    editorsViewConfig.add_child("registry.view", editorsView1Reg);
    ::fwServices::IService::ConfigType editorsView2Reg;
    editorsView2Reg.put("<xmlattr>.sid", "showScanEditor");
    editorsView2Reg.put("<xmlattr>.start", "yes");
    editorsViewConfig.add_child("registry.view", editorsView2Reg);
    ::fwServices::IService::ConfigType editorsView3Reg;
    editorsView3Reg.put("<xmlattr>.sid", "sliderIndexEditor");
    editorsView3Reg.put("<xmlattr>.start", "yes");
    editorsViewConfig.add_child("registry.view", editorsView3Reg);
    ::fwServices::IService::ConfigType editorsView4Reg;
    editorsView4Reg.put("<xmlattr>.sid", "snapshotEditor");
    editorsView4Reg.put("<xmlattr>.start", "yes");
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

    /* **************************************************************************************
    *              readers configuration
    ****************************************************************************************/

    imageReaderSrv->configure();
    meshReaderSrv->configure();
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

    sliderIndexEditor->registerInOut(m_image, "image", true);
    imageReaderSrv->registerInOut(m_image, "data");
    meshReaderSrv->registerInOut(m_mesh, "data");
    textureReaderSrv->registerInOut(m_texture, "data");
    imageAdaptor->registerInOut(m_image, "image", true);
    meshAdaptor->registerInput(m_mesh, "mesh", true);
    textureAdaptor->registerInOut(m_texture, "texture", true);

    /* **************************************************************************************
    *              connect the services
    ****************************************************************************************/

    meshReaderSrv->signal("jobCreated")->connect(progressBar->slot("showJob"));
    imageReaderSrv->signal("jobCreated")->connect(progressBar->slot("showJob"));
    textureReaderSrv->signal("jobCreated")->connect(progressBar->slot("showJob"));

    showScanEditor->signal("toggled")->connect(sliceListEditor->slot("setEnabled"));
    showScanEditor->signal("toggled")->connect(imageAdaptor->slot("showSlice"));

    snapshotEditor->signal("snapped")->connect(snapshotAdaptor->slot("snap"));

    sliceListEditor->signal("selected")->connect(imageAdaptor->slot("updateSliceMode"));

    meshAdaptor->signal("textureApplied")->connect(textureAdaptor->slot("applyTexture"));

    /* **************************************************************************************
    *              start the services
    ****************************************************************************************/
    frameSrv->start();
    imageAdaptor->start();
    meshAdaptor->start();
    textureAdaptor->start();
    snapshotAdaptor->start();
    m_startedService.emplace_back(frameSrv);
    m_startedService.emplace_back(imageAdaptor);
    m_startedService.emplace_back(meshAdaptor);
    m_startedService.emplace_back(textureAdaptor);
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
    m_image.reset();
    m_texture.reset();
    m_mesh.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto08GenericSceneCtrl
