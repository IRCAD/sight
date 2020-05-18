/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "guiQt/editor/SImage.hpp"

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

namespace guiQt
{
namespace editor
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::guiQt::editor::SImage );

//-----------------------------------------------------------------------------

SImage::SImage() noexcept
{
}

//-----------------------------------------------------------------------------

SImage::~SImage() noexcept
{
}

//-----------------------------------------------------------------------------

void SImage::starting()
{
    this->create();

    const ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* const layout = new QVBoxLayout();
    QLabel* const label       = new QLabel("");

    QPixmap* const pixmap = new QPixmap();

    pixmap->load(m_path.string().c_str());

    if(pixmap->isNull())
    {
        OSLM_ERROR("Image could not be found: " << m_path);
    }

    if(m_width != -1 && m_height != -1)
    {
        label->setPixmap(pixmap->scaled(m_width, m_height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else if(m_width != -1)
    {
        label->setPixmap((pixmap->scaledToWidth(m_width, Qt::SmoothTransformation)));
    }
    else if(m_height != -1)
    {
        label->setPixmap((pixmap->scaledToHeight(m_height, Qt::SmoothTransformation)));
    }
    else
    {
        label->setPixmap(*pixmap);
    }
    layout->addWidget(label);
    qtContainer->setLayout(layout);

}

//-----------------------------------------------------------------------------

void SImage::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SImage::configuring()
{
    this->initialize();

    const ConfigType cfg = this->getConfigTree();
    std::string pathCfg  = cfg.get< std::string >("path");

    m_path = ::fwRuntime::getBundleResourceFilePath(pathCfg);

    m_width  = cfg.get< int >("width", m_width);
    m_height = cfg.get< int >("height", m_height);

}

//-----------------------------------------------------------------------------

void SImage::updating()
{

}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui
