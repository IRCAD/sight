/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QtGui>
#include <QTabWidget>
#include <QBoxLayout>

#include <fwCore/base.hpp>

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/String.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Boolean.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwGui/GuiRegistry.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwDataCamp/getObject.hpp>

#include "guiQt/editor/DynamicView.hpp"

namespace guiQt
{
namespace editor
{

fwServicesRegisterMacro( ::gui::view::IView, ::guiQt::editor::DynamicView, ::fwData::Object );

AppConfig::AppConfig(const DynamicView::ConfigType& config) :
    id(config.get<std::string>("<xmlattr>.id")),
    title(config.get<std::string>("<xmlattr>.title"))
{
    std::string closableStr = config.get_optional<std::string>("<xmlattr>.closable").get_value_or("true");
    closable = (closableStr == "true");


    tabInfo = config.get_optional<std::string>("<xmlattr>.tabinfo").get_value_or("");

    if(config.count("parameters") == 1 )
    {
        const ConfigType &configParameters = config.get_child("parameters");
        BOOST_FOREACH( const ConfigType::value_type &v, configParameters.equal_range("parameter") )
        {
            ParameterType parameter( v.second );
            parameters.push_back( parameter );
        }
    }
    OSLM_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//------------------------------------------------------------------------------

DynamicView::DynamicView() throw()
{
    m_dynamicConfigStartStop = false;
    //addNewHandledEvent( "NEW_CONFIGURATION_HELPER" );
}

//------------------------------------------------------------------------------

DynamicView::~DynamicView() throw()
{
}

//------------------------------------------------------------------------------

void DynamicView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr parentContainer;
    parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QWidget* qtContainer = parentContainer->getQtContainer();
    m_tabWidget = new QTabWidget(qtContainer);
    m_tabWidget->setTabsClosable ( true );
    m_tabWidget->setDocumentMode( true );
    m_tabWidget->setMovable( true );

    QObject::connect(m_tabWidget, SIGNAL(tabCloseRequested( int )), this, SLOT( closeTabSignal( int )));
    QObject::connect(m_tabWidget, SIGNAL(currentChanged( int )), this, SLOT(changedTab( int )));


    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    if (qtContainer->layout())
    {
        QWidget().setLayout(qtContainer->layout());
    }
    qtContainer->setLayout(layout);

    layout->addWidget( m_tabWidget );
    m_currentWidget = 0;

    if(!m_appConfig.id.empty())
    {
        DynamicViewInfo info = this->buildDynamicViewInfo(m_appConfig);
        this->launchTab(info);
    }
}

//------------------------------------------------------------------------------

void DynamicView::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    while(m_tabWidget->count())
    {
        this->closeTab(0,true);
    }
    m_tabWidget->clear();
    ::fwGuiQt::container::QtContainer::sptr parentContainer;
    parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    parentContainer->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
    m_tabWidget = 0;
}

//------------------------------------------------------------------------------

void DynamicView::configuring() throw(fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();

    if(this->getConfigTree().get_child("service").count("config") > 0)
    {
        SLM_ASSERT("Sorry you must have one (and only one) <config/> element.",
                   this->getConfigTree().get_child("service").count("config") == 1 );
        const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree().get_child("service");
        const ::fwServices::IService::ConfigType &config   = srvconfig.get_child("config");

        const std::string dynamicConfig =
            config.get_optional<std::string>("<xmlattr>.dynamicConfigStartStop").get_value_or("false");
        m_dynamicConfigStartStop = (dynamicConfig == "true");

        if(config.count("appConfig") == 1 )
        {
            const ::fwServices::IService::ConfigType &appConfig = config.get_child("appConfig");
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

    ::fwData::Object::sptr currentObj    = this->getObject();
    ::fwData::Composite::sptr replaceMap = ::fwData::Composite::New();
    BOOST_FOREACH(const AppConfig::ParametersType::value_type& param, appConfig.parameters)
    {
        if(!param.isSeshat())
        {
            (*replaceMap)[param.replace] = ::fwData::String::New(param.by);
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0,1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(currentObj, parameterToReplace);
            OSLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0,1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            (*replaceMap)[param.replace] = ::fwData::String::New(parameterValue);
        }
    }
    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier(appConfig.id);
    (*replaceMap)["GENERIC_UID"] = ::fwData::String::New(genericUidAdaptor);
    info.replaceMap              = replaceMap;
    return info;
}

//------------------------------------------------------------------------------

void DynamicView::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void DynamicView::swapping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void DynamicView::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    if (msg->hasEvent("NEW_CONFIGURATION_HELPER"))
    {
        DynamicViewInfo info;
        ::fwData::String::csptr titleData =
            ::fwData::String::dynamicConstCast(msg->getDataInfo( "NEW_CONFIGURATION_HELPER" ) );

        const std::string eventID           = "NEW_CONFIGURATION_HELPER";
        const std::string fieldID           = "APPCONFIG";
        const std::string viewConfigFieldID = "VIEWCONFIGID";
        const std::string closableFieldID   = "CLOSABLE";
        const std::string iconFieldID       = "ICON";
        const std::string tooltipFieldID    = "TOOLTIP";
        const std::string tabIDFieldID      = "TABID";
        const std::string asFieldID         = "ACTIVITYSERIES";
        const std::string tabInfo           = "TABINFO";

        SLM_ASSERT("Missing field 'tabID' in message", titleData->getField(tabIDFieldID));
        info.title        = titleData->getField< ::fwData::String >("TABINFO")->value();
        info.tabID        = titleData->getField< ::fwData::String >(tabIDFieldID)->value();
        info.closable     = titleData->getField(closableFieldID, ::fwData::Boolean::New(true))->value();
        info.icon         = titleData->getField(iconFieldID, ::fwData::String::New(""))->value();
        info.tooltip      = titleData->getField(tooltipFieldID, ::fwData::String::New(""))->value();
        info.viewConfigID = titleData->getField(viewConfigFieldID, ::fwData::String::New(""))->value();
        info.replaceMap   = titleData->getField(fieldID, ::fwData::Composite::New());


        this->launchTab(info);
    }
}

//------------------------------------------------------------------------------

void DynamicView::launchTab(DynamicViewInfo& info)
{
    static int count = 0;
    if(m_tabIDList.find(info.tabID) != m_tabIDList.end() )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("New tab",
                                                          "Sorry, the tab " + info.title + " cannot be opened twice.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    if ( m_titleToCount.find( info.title ) !=  m_titleToCount.end() )
    {
        m_titleToCount[ info.title ]++;
    }
    else
    {
        m_titleToCount[ info.title ] = 1;
    }

    QString finalTitle = QString("%1 %2").arg( info.title.c_str(),"(%1)" ).arg( m_titleToCount[ info.title ] );
    info.wid = QString("DynamicView-%1").arg(count++).toStdString();

    ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
    QWidget *widget = new QWidget();
    subContainer->setQtContainer(widget);
    ::fwGui::GuiRegistry::registerWIDContainer(info.wid, subContainer);

    (*(info.replaceMap))[ "WID_PARENT" ] = fwData::String::New( info.wid );


    ::fwRuntime::ConfigurationElement::csptr config =
        ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( info.viewConfigID, info.replaceMap);

    ::fwServices::AppConfigManager::sptr helper = ::fwServices::AppConfigManager::New();
    helper->setConfig( config );

    if (!m_dynamicConfigStartStop)
    {
        helper->launch();
    }
    else
    {
        helper->create();
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

void DynamicView::info( std::ostream &_sstream )
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
    QWidget *widget = m_tabWidget->widget(index);

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

        info.container->clean();
        info.container->destroyContainer();
        info.container.reset();
        m_dynamicInfoMap.erase(widget);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Close tab",
                                                          "Sorry, the tab " + info.title + " can not be closed.",
                                                          ::fwGui::dialog::IMessageDialog::INFO);
    }
}

//------------------------------------------------------------------------------

void DynamicView::changedTab( int index )
{
    QWidget *widget = m_tabWidget->widget(index);

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

