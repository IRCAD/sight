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

#include <fwCore/base.hpp>

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

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    QIcon* image        = new QIcon(m_path.string().c_str());
    QLabel* label       = new QLabel("");
    if(image->isNull())
    {
        OSLM_ERROR("Image could not be found: " << m_path);
    }
    QPixmap* pixmap = new QPixmap();
    pixmap->load(m_path.string().c_str());
    if(m_width != -1 && m_height != -1)
    {
        label->setPixmap(pixmap->scaled(m_width, m_height));
    }
    else if(m_width != -1)
    {
        label->setPixmap((pixmap->scaledToWidth(m_width)));
    }
    else if(m_height != -1)
    {
        label->setPixmap((pixmap->scaledToHeight(m_height)));
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

    m_width  = cfg.get_optional< int >("width").get_value_or(m_width);
    m_height = cfg.get_optional< int >("height").get_value_or(m_height);

}

//-----------------------------------------------------------------------------

void SImage::updating()
{

}

//-----------------------------------------------------------------------------

void SImage::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui
