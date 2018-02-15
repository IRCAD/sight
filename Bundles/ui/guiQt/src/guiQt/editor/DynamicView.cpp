/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/DynamicView.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <boost/foreach.hpp>

#include <QBoxLayout>
#include <QTabWidget>
#include <QtGui>

namespace guiQt
{
namespace editor
{

static const ::fwCom::Slots::SlotKeyType s_CREATE_TAB_SLOT = "createTab";

fwServicesRegisterMacro( ::fwGui::view::IView, ::guiQt::editor::DynamicView);

AppConfig::AppConfig(const ConfigType& config) :
    id(config.get<std::string>("<xmlattr>.id")),
    title(config.get<std::string>("<xmlattr>.title"))
{
    std::string closableStr = config.get_optional<std::string>("<xmlattr>.closable").get_value_or("true");
    closable = (closableStr == "true");

    tabInfo = config.get_optional<std::string>("<xmlattr>.tabinfo").get_value_or("");

    if(config.count("parameters") == 1 )
    {
        const ConfigType& configParameters = config.get_child("parameters");
        BOOST_FOREACH( const ConfigType::value_type& v,  configParameters.equal_range("parameter") )
        {
            ParameterType parameter( v.second );
            parameters.push_back( parameter );
        }
    }
    OSLM_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//------------------------------------------------------------------------------

DynamicView::DynamicView() noexcept
{
    m_dynamicConfigStartStop = false;

    newSlot(s_CREATE_TAB_SLOT, &DynamicView::createTab, this);
}

//------------------------------------------------------------------------------

DynamicView::~DynamicView() noexcept
{
}

//------------------------------------------------------------------------------

void DynamicView::starting()
{
    SLM_TRACE_FUNC();

    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr parentContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable( true );
    m_tabWidget->setDocumentMode( true );
    m_tabWidget->setMovable( true );

    QObject::connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabSignal(int)));
    QObject::connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int)));

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget( m_tabWidget );

    parentContainer->setLayout(layout);

    m_currentWidget = 0;

    if(!m_appConfig.id.empty())
    {
        DynamicViewInfo info = this->buildDynamicViewInfo(m_appConfig);
        this->launchTab(info);
    }
}

//------------------------------------------------------------------------------

void DynamicView::stopping()
{
    SLM_TRACE_FUNC();
    while(m_tabWidget->count())
    {
        this->closeTab(0, true);
    }
    m_tabWidget->clear();

    this->destroy();
    m_tabWidget = 0;
}

//------------------------------------------------------------------------------

void DynamicView::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();

    if(this->getConfigTree().count("config") > 0)
    {
        SLM_ASSERT("There must be one (and only one) <config/> element.",
                   this->getConfigTree().count("config") == 1 );
        const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree();
        const ::fwServices::IService::ConfigType& config   = srvconfig.get_child("config");

        const std::string dynamicConfig =
            config.get_optional<std::string>("<xmlattr>.dynamicConfigStartStop").get_value_or("false");
        m_dynamicConfigStartStop = (dynamicConfig == "true");

        if(config.count("appConfig") == 1 )
        {
            const ::fwServices::IService::ConfigType& appConfig = config.get_child("appConfig");
            m_appConfig = AppConfig(appConfig);
        }
        OSLM_ASSERT("At most 1 <appConfig> tag is allowed", config.count("appConfig") < 2);
    }
}

//------------------------------------------------------------------------------

DynamicView::DynamicViewInfo DynamicView::buildDynamicViewInfo(const AppConfig& appConfig)
{
    DynamicViewInfo info;
    info.tabID = "TABID_" + this->getID();

    if(appConfig.tabInfo.empty())
    {
        info.title = appConfig.title;
    }
    else
    {
        info.title = appConfig.tabInfo;
    }
    info.viewConfigID = appConfig.id;
    info.closable     = appConfig.closable;

    ::fwData::Object::sptr currentObj = this->getObject();
    ReplaceMapType replaceMap;
    for(const AppConfig::ParametersType::value_type& param :  appConfig.parameters)
    {
        if(!param.isSeshat())
        {
            replaceMap[param.replace] = param.by;
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(currentObj, parameterToReplace);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            replaceMap[param.replace] = parameterValue;
        }
    }
    info.replaceMap = replaceMap;
    return info;
}

//------------------------------------------------------------------------------

void DynamicView::updating()
{
}

//------------------------------------------------------------------------------

void DynamicView::swapping()
{
}

//------------------------------------------------------------------------------

void DynamicView::createTab( ::fwActivities::registry::ActivityMsg info )
{
    DynamicViewInfo viewInfo;
    viewInfo.title        = info.getTitle();
    viewInfo.tabID        = info.getTabID();
    viewInfo.closable     = info.isClosable();
    viewInfo.icon         = info.getIconPath();
    viewInfo.tooltip      = info.getToolTip();
    viewInfo.viewConfigID = info.getAppConfigID();
    viewInfo.replaceMap   = info.getReplaceMap();

    this->launchTab(viewInfo);
}

//------------------------------------------------------------------------------

void DynamicView::launchTab(DynamicViewInfo& info)
{
    static int count = 0;
    if(m_tabIDList.find(info.tabID) != m_tabIDList.end() )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("New tab",
                                                          "The tab " + info.title + " cannot be opened twice.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    if ( m_titleToCount.find( info.title ) != m_titleToCount.end() )
    {
        m_titleToCount[ info.title ]++;
    }
    else
    {
        m_titleToCount[ info.title ] = 1;
    }

    QString finalTitle = QString("%1 %2").arg( info.title.c_str(), "(%1)" ).arg( m_titleToCount[ info.title ] );
    info.wid = QString("DynamicView-%1").arg(count++).toStdString();

    ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
    QWidget* widget = new QWidget();
    subContainer->setQtContainer(widget);
    ::fwGui::GuiRegistry::registerWIDContainer(info.wid, subContainer);

    info.replaceMap[ "WID_PARENT" ] = info.wid;
    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier(info.viewConfigID);
    info.replaceMap["GENERIC_UID"] = genericUidAdaptor;

    ::fwServices::IAppConfigManager::sptr helper = ::fwServices::IAppConfigManager::New();
    helper->setConfig( info.viewConfigID, info.replaceMap );

    try
    {
        if (!m_dynamicConfigStartStop)
        {
            helper->launch();
        }
        else
        {
            helper->create();
        }
    }
    catch( std::exception& e )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launch failed",
                                                          e.what(),
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
        OSLM_ERROR(e.what());
    }

    info.container = subContainer;
    info.helper    = helper;

    m_dynamicInfoMap[widget] = info;
    m_tabIDList.insert(info.tabID);

    int index = m_tabWidget->addTab(widget, finalTitle );
    if(!info.tooltip.empty())
    {
        m_tabWidget->setTabToolTip(index, QString::fromStdString(info.tooltip));
    }
    if(!info.icon.empty())
    {
        m_tabWidget->setTabIcon(index, QIcon(QString::fromStdString(info.icon)) );
    }
    m_tabWidget->setCurrentWidget(widget);
}

//------------------------------------------------------------------------------

void DynamicView::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void DynamicView::closeTabSignal( int index )
{
    closeTab( index, false );
}

//------------------------------------------------------------------------------

void DynamicView::closeTab( int index, bool forceClose )
{
    QWidget* widget = m_tabWidget->widget(index);

    SLM_ASSERT("Widget is not in dynamicInfoMap", m_dynamicInfoMap.find(widget) != m_dynamicInfoMap.end());
    DynamicViewInfo info = m_dynamicInfoMap[widget];
    if ( info.closable || forceClose )
    {
        m_tabIDList.erase(info.tabID);
        if (!m_dynamicConfigStartStop)
        {
            info.helper->stopAndDestroy();
        }
        else
        {
            if (info.helper->isStarted())
            {
                info.helper->stop();
            }
            info.helper->destroy();
        }
        info.helper.reset();

        //Remove tab first, to avoid tab beeing removed by container->destroy
        m_currentWidget = 0;
        m_tabWidget->removeTab(index);

        ::fwGui::GuiRegistry::unregisterWIDContainer(info.wid);

        info.container->destroyContainer();
        info.container.reset();
        m_dynamicInfoMap.erase(widget);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Close tab",
                                                          "The tab " + info.title + " can not be closed.",
                                                          ::fwGui::dialog::IMessageDialog::INFO);
    }
}

//------------------------------------------------------------------------------

void DynamicView::changedTab( int index )
{
    QWidget* widget = m_tabWidget->widget(index);

    if (m_dynamicConfigStartStop && widget != m_currentWidget)
    {
        if (m_currentWidget)
        {
            DynamicViewInfo oldinfo = m_dynamicInfoMap[m_currentWidget];
            oldinfo.helper->stop();
        }

        if (widget)
        {
            DynamicViewInfo newinfo = m_dynamicInfoMap[widget];
            if (!newinfo.helper->isStarted())
            {
                newinfo.helper->start();
                newinfo.helper->update();
            }
        }

    }

    m_currentWidget = widget;
}

//------------------------------------------------------------------------------

}// namespace editor
}// namespace guiQt
