/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QStyle>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwGuiQt/layoutManager/LineLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IViewLayoutManager,
        ::fwGui::LineLayoutManager,
        ::fwGui::layoutManager::LineLayoutManagerBase::RegistryKeyType,
        ::fwGui::layoutManager::LineLayoutManagerBase::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

LineLayoutManager::LineLayoutManager()
{}

//-----------------------------------------------------------------------------

LineLayoutManager::~LineLayoutManager()
{}

//-----------------------------------------------------------------------------

void LineLayoutManager::createLayout( ::fwGui::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *qtContainer = m_parentContainer->getQtContainer();
    Orientation orientation = this->getOrientation();

    if( orientation == VERTICAL )
    {
        layout->setDirection(QBoxLayout::TopToBottom);
    }

    if(qtContainer->layout() && qobject_cast<QVBoxLayout*> ( qtContainer->layout() ) )
    {
        QVBoxLayout * vLayout = qobject_cast<QVBoxLayout*> ( qtContainer->layout() );
        vLayout->addLayout(layout);
    }
    else
    {
        if (qtContainer->layout())
        {
            qtContainer->layout()->deleteLater();
        }
        qtContainer->setLayout(layout);
    }

    const std::list< ViewInfo> &views = this->getViewsInfo();
    BOOST_FOREACH ( ViewInfo viewInfo, views)
    {
        QWidget *panel;
        int border = viewInfo.m_border;
        if(viewInfo.m_caption.first)
        {
            QGroupBox *groupbox = new QGroupBox(qtContainer);
            groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
            panel = groupbox;
            border += groupbox->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
        }
        else
        {
            panel = new QWidget(qtContainer);
        }
        panel->setMinimumSize(viewInfo.m_minSize.first, viewInfo.m_minSize.second);
        panel->setContentsMargins(border, border,border, border);

        ::fwGuiQt::container::QtContainer::NewSptr subContainer;
        subContainer->setQtContainer(panel);
        m_subViews.push_back(subContainer);

        layout->addWidget( panel );
        layout->setStretchFactor(panel, viewInfo.m_proportion);
    }
}

//-----------------------------------------------------------------------------

void LineLayoutManager::destroyLayout()
{
    this->destroySubViews();
    QWidget *qtContainer = m_parentContainer->getQtContainer();
    qtContainer->layout()->deleteLater();
    qtContainer->setLayout(0);
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace fwGui



