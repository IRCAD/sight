/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ui/qt/layoutManager/LineLayoutManager.hpp"

#include "ui/qt/App.hpp"

#include <core/base.hpp>

#include <data/tools/Color.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QStyle>

#include <ui/base/registry/macros.hpp>

fwGuiRegisterMacro( ::sight::ui::base::LineLayoutManager,
                    ::sight::ui::base::layoutManager::LineLayoutManagerBase::REGISTRY_KEY )

namespace sight::ui::base
{

//-----------------------------------------------------------------------------

LineLayoutManager::LineLayoutManager(ui::base::GuiBaseObject::Key)
{
}

//-----------------------------------------------------------------------------

LineLayoutManager::~LineLayoutManager()
{
}

//-----------------------------------------------------------------------------

void LineLayoutManager::createLayout( ui::base::container::fwContainer::sptr parent )
{
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(parent);
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
            int leftBorder;
            int topBorder;
            int rightBorder;
            int bottomBorder;

            if(viewInfo.m_spacing != -1)
            {
                layout->setSpacing(viewInfo.m_spacing);
            }

            if(viewInfo.m_border != 0)
            {
                leftBorder = topBorder = rightBorder = bottomBorder = viewInfo.m_border;
            }
            else
            {
                leftBorder   = viewInfo.m_leftBorder;
                topBorder    = viewInfo.m_topBorder;
                rightBorder  = viewInfo.m_rightBorder;
                bottomBorder = viewInfo.m_bottomBorder;
            }

            if(viewInfo.m_caption.first)
            {
                QGroupBox* groupbox = new QGroupBox();
                groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
                panel         = groupbox;
                leftBorder   += groupbox->style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
                topBorder    += groupbox->style()->pixelMetric(QStyle::PM_LayoutTopMargin);
                rightBorder  += groupbox->style()->pixelMetric(QStyle::PM_LayoutRightMargin);
                bottomBorder += groupbox->style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
            }
            else
            {
                panel = new QWidget();
            }
            panel->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
            panel->setContentsMargins(leftBorder, topBorder, rightBorder, bottomBorder);
            if(!viewInfo.m_toolTip.empty())
            {
                panel->setToolTip(QString::fromStdString(viewInfo.m_toolTip));
            }

            ui::qt::container::QtContainer::sptr subContainer = ui::qt::container::QtContainer::New();
            subContainer->setQtContainer(panel);
            m_subViews.push_back(subContainer);

            if(!viewInfo.m_backgroundColor.empty())
            {
                std::uint8_t rgba[4];
                data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast< short >(rgba[0]) << ','
                   << static_cast< short >(rgba[1]) << ','
                   << static_cast< short >(rgba[2]) << ','
                   << (static_cast< float >(rgba[3])/255.f)*100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                panel->setStyleSheet(style + qApp->styleSheet());
            }

            if(viewInfo.m_useScrollBar)
            {
                QScrollArea* scrollArea = new QScrollArea();
                scrollArea->setWidget(panel);
                scrollArea->setWidgetResizable( true );
                if(!viewInfo.m_backgroundColor.empty())
                {
                    std::uint8_t rgba[4];
                    data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                    std::stringstream ss;
                    ss << "QWidget { background-color: rgba(" << static_cast< short >(rgba[0]) << ','
                       << static_cast< short >(rgba[1]) << ','
                       << static_cast< short >(rgba[2]) << ','
                       << (static_cast< float >(rgba[3])/255.f)*100 << "%); } ";
                    const QString style = QString::fromStdString(ss.str());
                    scrollArea->setStyleSheet(style + qApp->styleSheet());
                }

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

} // namespace sight::ui::base
