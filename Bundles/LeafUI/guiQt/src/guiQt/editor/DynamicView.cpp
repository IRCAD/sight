/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QtGui>

#include <fwCore/base.hpp>

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/Object.hpp>


#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Boolean.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwGui/GuiRegistry.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>


#include "guiQt/editor/DynamicView.hpp"

namespace guiQt
{
namespace editor
{

REGISTER_SERVICE( ::gui::view::IView , ::guiQt::editor::DynamicView , ::fwTools::Object ) ;

DynamicView::DynamicView() throw()
{
    m_dynamicConfigStartStop = false;
    addNewHandledEvent( "NEW_CONFIGURATION_HELPER" );
}

//------------------------------------------------------------------------------

DynamicView::~DynamicView() throw()
{}

//------------------------------------------------------------------------------

void DynamicView::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr parentContainer ;
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
        qtContainer->layout()->deleteLater();
    }
    qtContainer->setLayout(layout);

    layout->addWidget( m_tabWidget );
    m_currentWidget = 0;
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
    ::fwGuiQt::container::QtContainer::sptr parentContainer ;
    parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    parentContainer->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
    m_tabWidget = 0;
}

//------------------------------------------------------------------------------

void DynamicView::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void DynamicView::updating() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void DynamicView::swapping() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void DynamicView::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    static int count = 0;

    if (_msg->hasEvent("NEW_CONFIGURATION_HELPER"))
    {
        ::fwData::String::csptr titleData = ::fwData::String::dynamicConstCast( _msg->getDataInfo( "NEW_CONFIGURATION_HELPER" ) );
        SLM_ASSERT("Missing field 'tabID' in message", titleData->getField_NEWAPI("tabID"));
        std::string tabID = titleData->getField_NEWAPI< ::fwData::String >("tabID")->value();
        std::string title = titleData->value();
        if(m_tabIDList.find(tabID) != m_tabIDList.end() )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("New tab",
                    "Sorry, the tab " + title + " cannot be opened twice.",
                    ::fwGui::dialog::IMessageDialog::WARNING);
            return;
        }
        bool closable;
        std::string icon;
        std::string tooltip;
        std::string viewConfigID;
        ::fwData::Composite::sptr fieldAdaptors;

        closable      = titleData->getField_NEWAPI("closable", ::fwData::Boolean::New(true))->value();
        icon          = titleData->getField_NEWAPI("icon", ::fwData::String::New(""))->value();
        tooltip       = titleData->getField_NEWAPI("tooltip", ::fwData::String::New(""))->value();
        viewConfigID  = titleData->getField_NEWAPI("viewConfigID", ::fwData::String::New(""))->value();
        fieldAdaptors = titleData->getField_NEWAPI("::fwServices::registry::AppConfig", ::fwData::Composite::New());

        if ( m_titleToCount.find( title ) !=  m_titleToCount.end() )
        {
            m_titleToCount[ title ] ++;
        }
        else
        {
            m_titleToCount[ title ] = 1;
        }

        QString finalTitle = QString("%1 %2").arg( title.c_str(),"(%1)" ).arg( m_titleToCount[ title ] );

        std::string wid = QString("DynamicView-%1").arg(count++).toStdString();

        ::fwGuiQt::container::QtContainer::NewSptr subContainer;
        QWidget *widget = new QWidget();
        subContainer->setQtContainer(widget);
        ::fwGui::GuiRegistry::registerWIDContainer(wid, subContainer);

        (*fieldAdaptors)[ "WID_PARENT" ] = fwData::String::New( wid );


        ::fwRuntime::ConfigurationElement::csptr config =
                ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( viewConfigID, fieldAdaptors );

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

        DynamicViewInfo info;
        info.container = subContainer;
        info.helper    = helper;
        info.wid       = wid;
        info.title     = title;
        info.closable  = closable;
        info.icon      = icon;
        info.tooltip   = tooltip;
        info.tabID     = tabID;

        m_dynamicInfoMap[widget] = info;
        m_tabIDList.insert(tabID);

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
}

//------------------------------------------------------------------------------

void DynamicView::info( std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void DynamicView::closeTabSignal( int index )
{
    closeTab( index, false );
}

//------------------------------------------------------------------------------

void DynamicView::closeTab( int index, bool forceClose )
{
    QWidget *widget = m_tabWidget->widget(index);
    DynamicViewInfo info = m_dynamicInfoMap[widget];
    if ( info.closable || forceClose )
    {
        m_dynamicInfoMap.erase(widget);
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

        ::fwGui::GuiRegistry::unregisterWIDContainer(info.wid);

        //info.container->clean();
        info.container->destroyContainer();
        info.container.reset();

        m_currentWidget = 0;
        m_tabWidget->removeTab(index);
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

