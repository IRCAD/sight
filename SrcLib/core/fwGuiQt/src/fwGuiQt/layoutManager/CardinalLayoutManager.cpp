/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMainWindow>
#include <QDockWidget>
#include <QBoxLayout>
#include <QGroupBox>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/layoutManager/CardinalLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IViewLayoutManager,
        ::fwGui::CardinalLayoutManager,
        ::fwGui::layoutManager::CardinalLayoutManagerBase::RegistryKeyType,
        ::fwGui::layoutManager::CardinalLayoutManagerBase::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

CardinalLayoutManager::CardinalLayoutManager()
{}

//-----------------------------------------------------------------------------

CardinalLayoutManager::~CardinalLayoutManager()
{}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::createLayout( ::fwGui::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    QWidget *qtContainer = m_parentContainer->getQtContainer();
    m_qtWindow = new QMainWindow( );
    const std::list< ViewInfo> &views = this->getViewsInfo();

    bool hasCentral = false;

    BOOST_FOREACH ( ViewInfo viewInfo, views)
    {
        QWidget *widget;
        QWidget *insideWidget;

        if(viewInfo.m_align==CENTER)
        {
            if(viewInfo.m_caption.first)
            {
                QGroupBox *groupbox = new QGroupBox(qtContainer);
                groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
                insideWidget = groupbox;
            }
            else
            {
                insideWidget = new QWidget(qtContainer);
            }

            widget = insideWidget;
            SLM_ASSERT("multiple center views are not managed in Qt version of CardinalLayoutManager",
                    !hasCentral);

            m_qtWindow->setCentralWidget(widget);
            hasCentral = true;
        }
        else
        {
            insideWidget = new QWidget(qtContainer);
            QDockWidget *dockWidget= new QDockWidget(0);
            QDockWidget::DockWidgetFeatures features;

            features = QDockWidget::DockWidgetMovable ;

            Qt::DockWidgetArea area;

            if(viewInfo.m_align==RIGHT)       { area = Qt::RightDockWidgetArea;}
            else if(viewInfo.m_align==LEFT)   { area = Qt::LeftDockWidgetArea;}
            else if(viewInfo.m_align==BOTTOM) { area = Qt::BottomDockWidgetArea;}
            else if(viewInfo.m_align==TOP)    { area = Qt::TopDockWidgetArea;}

            m_qtWindow->addDockWidget(area, dockWidget);
            dockWidget->setFeatures(features);

            if(viewInfo.m_caption.first)
            {
                dockWidget->setWindowTitle( QString::fromStdString(viewInfo.m_caption.second) );
            }

            dockWidget->setWidget(insideWidget);
        }

        if(!viewInfo.m_isResizable)
        {
            //TODO
        }

        insideWidget->setMinimumSize(viewInfo.m_minSize.first, viewInfo.m_minSize.second);
        insideWidget->setVisible(viewInfo.m_visible);
        //TODO
        // - viewInfo.m_position
        // - viewInfo.m_layer
        // - viewInfo.m_row
        // - viewInfo.m_visible

        ::fwGuiQt::container::QtContainer::NewSptr subContainer;
        subContainer->setQtContainer(insideWidget);
        m_subViews.push_back(subContainer);

    }

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
    if(qtContainer->layout())
    {
        qtContainer->layout()->deleteLater();
    }
    qtContainer->setLayout(layout);
    m_qtWindow->setParent(qtContainer);
    m_qtWindow->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget( m_qtWindow );


    m_qtWindow->show();
}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
    m_qtWindow->hide();
    m_qtWindow->setParent(0);
    m_qtWindow->deleteLater();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



