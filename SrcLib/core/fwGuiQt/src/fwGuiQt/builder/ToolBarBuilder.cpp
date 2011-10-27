/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QMainWindow>
#include <QToolBar>
#include <QHBoxLayout>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/container/QtToolBarContainer.hpp"
#include "fwGuiQt/builder/ToolBarBuilder.hpp"



REGISTER_BINDING( ::fwGui::builder::IToolBarBuilder,
        ::fwGui::builder::ToolBarBuilder,
        ::fwGui::builder::IToolBarBuilder::RegistryKeyType,
        ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY );


namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

ToolBarBuilder::ToolBarBuilder()
{}

//-----------------------------------------------------------------------------

ToolBarBuilder::~ToolBarBuilder()
{}

//-----------------------------------------------------------------------------

void ToolBarBuilder::createToolBar( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("Sorry, the parent container is not a QtContainer", m_parent);
    QMainWindow *window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer() ) ;

    QToolBar *toolBar = new QToolBar(QObject::tr("ToolBar"));
    toolBar->setIconSize( QSize(m_toolBitmapSize.first, m_toolBitmapSize.second) );
    toolBar->setFloatable(false);

    ::fwGuiQt::container::QtToolBarContainer::NewSptr toolBarContainer;
    if (window)
    {
        bool visible = window->isVisible();

        Qt::ToolBarArea area;
        switch (m_aligment)
        {
        case TOP:
            area = Qt::TopToolBarArea;
            break;
        case BOTTOM:
            area = Qt::BottomToolBarArea;
            break;
        case RIGHT:
            area = Qt::RightToolBarArea;
            break;
        case LEFT:
            area = Qt::LeftToolBarArea;
            break;
        }
        window->addToolBar( area, toolBar );

        //on Os X, the window is hidden (???)
        window->setVisible(visible);
    }
    else // parent is not a QMainWindow
    {
        QWidget * widget = m_parent->getQtContainer();
        SLM_ASSERT("Parent container must have a layout", widget->layout());
        QBoxLayout * layout = qobject_cast<QBoxLayout*> ( widget->layout() );
        switch (m_aligment)
        {
        case TOP:
            layout->setDirection(QBoxLayout::TopToBottom);
            toolBar->setOrientation(Qt::Horizontal);
            break;
        case BOTTOM:
            layout->setDirection(QBoxLayout::BottomToTop);
            toolBar->setOrientation(Qt::Horizontal);
            break;
        case RIGHT:
            layout->setDirection(QBoxLayout::RightToLeft);
            toolBar->setOrientation(Qt::Vertical);
            break;
        case LEFT:
            layout->setDirection(QBoxLayout::LeftToRight);
            toolBar->setOrientation(Qt::Vertical);
            break;
        }
        SLM_ASSERT("Parent container layout must have be a QVBoxLayout", layout);
        layout->insertWidget(0, toolBar, 0);
    }

    toolBarContainer->setQtToolBar(toolBar);
    m_toolBar = toolBarContainer;

}

//-----------------------------------------------------------------------------

void ToolBarBuilder::destroyToolBar()
{
    SLM_ASSERT("Sorry, ToolBar not initialized", m_toolBar);
    SLM_ASSERT("Sorry, the parent container is not a QtContainer", m_parent);
    QMainWindow *window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer() ) ;

    if (window)
    {
        ::fwGuiQt::container::QtToolBarContainer::sptr qtToolBar = ::fwGuiQt::container::QtToolBarContainer::dynamicCast(m_toolBar);
        QToolBar * toolbar = qtToolBar->getQtToolBar();
        window->removeToolBar( toolbar );
    }
    m_toolBar->destroyContainer();
}

//-----------------------------------------------------------------------------


} // namespace builder
} // namespace fwGui



