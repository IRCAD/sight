/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/layoutManager/ToolboxLayoutManager.hpp"

#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

fwGuiRegisterMacro( ::fwGui::ToolboxLayoutManager, ::fwGui::layoutManager::ToolboxLayoutManagerBase::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

ToolboxLayoutManager::ToolboxLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ToolboxLayoutManager::~ToolboxLayoutManager()
{
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();

    ::fwGuiQt::container::QtContainer::sptr parentContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(parent);

    QVBoxLayout* layout = new QVBoxLayout();
    parentContainer->setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* sv = new QScrollArea();
    ::fwGuiQt::widget::QfwToolBox* toolbox = new ::fwGuiQt::widget::QfwToolBox(sv);
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);
    layout->addWidget(sv);

    const std::list< ViewInfo>& views = this->getViewsInfo();
    for ( ViewInfo viewInfo : views)
    {
        int border     = viewInfo.m_border;
        QWidget* panel = new QWidget();
        panel->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
        panel->setContentsMargins(border, border, border, border);

        ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
        subContainer->setQtContainer(panel);
        m_subViews.push_back(subContainer);

        int index = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea* scrollArea = new QScrollArea(toolbox);
            scrollArea->setWidget(panel);
            scrollArea->setWidgetResizable( true );
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

        if(false == viewInfo.m_visible)
        {
            subContainer->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::destroyLayout()
{
    this->destroySubViews();
}

//-----------------------------------------------------------------------------

} // namespace fwGui

