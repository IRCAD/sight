/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/qt/viz/snapshot_editor.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace sight::module::ui::qt::viz
{

//------------------------------------------------------------------------------

const core::com::signals::key_t snapshot_editor::SNAPPED_SIG = "snapped";

//------------------------------------------------------------------------------

snapshot_editor::snapshot_editor() noexcept
{
    m_sigSnapped = new_signal<SnappedSignalType>(SNAPPED_SIG);
}

//------------------------------------------------------------------------------

snapshot_editor::~snapshot_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void snapshot_editor::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    std::filesystem::path path = core::runtime::get_module_resource_file_path(
        "sight::module::ui::qt",
        "camera-photo.png"
    );
    QIcon icon(QString::fromStdString(path.string()));
    m_snapButton = new QPushButton(icon, "");
    m_snapButton->setToolTip(QObject::tr("Snapshot"));

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(m_snapButton);
    h_layout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(h_layout);

    QObject::connect(m_snapButton, SIGNAL(clicked()), this, SLOT(onSnapButton()));
}

//------------------------------------------------------------------------------

void snapshot_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void snapshot_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void snapshot_editor::updating()
{
}

//------------------------------------------------------------------------------

void snapshot_editor::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void snapshot_editor::onSnapButton()
{
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);
    if(container->isVisible())
    {
        std::string filename = sight::module::ui::qt::viz::snapshot_editor::requestFileName();

        if(!filename.empty())
        {
            m_sigSnapped->async_emit(filename);
        }
    }
    else
    {
        std::string msgInfo("It is not possible to snapshot the negato view. This view is not shown on screen.");
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Negato view snapshot");
        messageBox.setMessage(msgInfo);
        messageBox.setIcon(sight::ui::dialog::message::WARNING);
        messageBox.addButton(sight::ui::dialog::message::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

std::string snapshot_editor::requestFileName()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();
    std::string fileName;

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle("Save snapshot as");
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialogFile.addFilter("jpeg", "*.jpg *.jpeg");
    dialogFile.addFilter("bmp", "*.bmp");
    dialogFile.addFilter("png", "*.png");
    dialogFile.addFilter("tiff", "*.tiff");
    dialogFile.addFilter("all", "*.*");
    dialogFile.setOption(sight::ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        fileName = result->get_file().string();
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }

    return fileName;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
