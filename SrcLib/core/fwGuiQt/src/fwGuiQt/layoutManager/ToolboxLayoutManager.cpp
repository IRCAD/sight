/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QStyle>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwGuiQt/layoutManager/ToolboxLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IViewLayoutManager,
        ::fwGui::ToolboxLayoutManager,
        ::fwGui::layoutManager::ToolboxLayoutManagerBase::RegistryKeyType,
        ::fwGui::layoutManager::ToolboxLayoutManagerBase::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

ToolboxLayoutManager::ToolboxLayoutManager()
{}

//-----------------------------------------------------------------------------

ToolboxLayoutManager::~ToolboxLayoutManager()
{}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *qtContainer = m_parentContainer->getQtContainer();
    if (qtContainer->layout())
    {
        qtContainer->layout()->deleteLater();
    }
    qtContainer->setLayout(layout);

    ::fwGuiQt::widget::QfwToolBox* toolbox = new ::fwGuiQt::widget::QfwToolBox(qtContainer);
    QScrollArea* sv = new QScrollArea(qtContainer);
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);

    const std::list< ViewInfo> &views = this->getViewsInfo();
    BOOST_FOREACH ( ViewInfo viewInfo, views)
    {
        int border = viewInfo.m_border;
        QWidget* panel = new QWidget();
        panel->setMinimumSize(std::max(viewInfo.m_minSize.first,0), std::max(viewInfo.m_minSize.second,0));
        panel->setContentsMargins(border, border,border, border);

        ::fwGuiQt::container::QtContainer::NewSptr subContainer;
        subContainer->setQtContainer(panel);
        m_subViews.push_back(subContainer);

        int index = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea *scrollArea = new QScrollArea();
            scrollArea->setWidget(panel);
            scrollArea->setWidgetResizable ( true );
            index = toolbox->addItem(scrollArea, QString::fromStdString(viewInfo.m_caption));
        }
        else
        {
            index = toolbox->addItem(panel, QString::fromStdString(viewInfo.m_caption));
        }

        if(viewInfo.m_expanded)
        {
            toolbox->expandItem(index);
        }
        subContainer->setVisible( viewInfo.m_visible );
    }
    layout->addWidget(sv);
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::destroyLayout()
{
    this->destroySubViews();
    QWidget *qtContainer = m_parentContainer->getQtContainer();
    qtContainer->layout()->deleteLater();
    qtContainer->setLayout(0);
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



