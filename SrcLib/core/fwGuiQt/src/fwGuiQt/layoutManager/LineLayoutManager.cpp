/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/layoutManager/LineLayoutManager.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QStyle>

fwGuiRegisterMacro( ::fwGui::LineLayoutManager, ::fwGui::layoutManager::LineLayoutManagerBase::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

LineLayoutManager::LineLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

LineLayoutManager::~LineLayoutManager()
{
}

//-----------------------------------------------------------------------------

void LineLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    Orientation orientation = this->getOrientation();

    if( orientation == VERTICAL )
    {
        layout->setDirection(QBoxLayout::TopToBottom);
    }

    const std::list< ViewInfo>& views = this->getViewsInfo();
    for ( ViewInfo viewInfo : views)
    {
        if(viewInfo.m_isSpacer)
        {
            layout->addStretch();
        }
        else
        {
            QWidget* panel;
            int border = viewInfo.m_border;
            if(viewInfo.m_caption.first)
            {
                QGroupBox* groupbox = new QGroupBox();
                groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
                panel   = groupbox;
                border += groupbox->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
            }
            else
            {
                panel = new QWidget();
            }
            panel->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
            panel->setContentsMargins(border, border, border, border);

            ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
            subContainer->setQtContainer(panel);
            m_subViews.push_back(subContainer);

            if(viewInfo.m_useScrollBar)
            {
                QScrollArea* scrollArea = new QScrollArea();
                scrollArea->setWidget(panel);
                scrollArea->setWidgetResizable( true );

                layout->addWidget( scrollArea );
                layout->setStretchFactor(scrollArea, viewInfo.m_proportion);
            }
            else
            {
                layout->addWidget( panel );
                layout->setStretchFactor(panel, viewInfo.m_proportion);
            }

            if(false == viewInfo.m_visible)
            {
                subContainer->setVisible(false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void LineLayoutManager::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace fwGui

