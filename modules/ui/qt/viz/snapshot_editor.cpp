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
    m_sig_snapped = new_signal<snapped_signal_t>(SNAPPED_SIG);
}

//------------------------------------------------------------------------------

snapshot_editor::~snapshot_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void snapshot_editor::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    std::filesystem::path path = core::runtime::get_module_resource_file_path(
        "sight::module::ui::qt",
        "camera-photo.png"
    );
    QIcon icon(QString::fromStdString(path.string()));
    m_snap_button = new QPushButton(icon, "");
    m_snap_button->setToolTip(QObject::tr("Snapshot"));

    auto* h_layout = new QHBoxLayout();
    h_layout->addWidget(m_snap_button);
    h_layout->setContentsMargins(0, 0, 0, 0);

    qt_container->set_layout(h_layout);

    QObject::connect(m_snap_button, &QPushButton::clicked, this, &self_t::on_snap_button);
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

void snapshot_editor::on_snap_button()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* container = qt_container->get_qt_container();
    SIGHT_ASSERT("container not instanced", container);
    if(container->isVisible())
    {
        std::string filename = sight::module::ui::qt::viz::snapshot_editor::request_file_name();

        if(!filename.empty())
        {
            m_sig_snapped->async_emit(filename);
        }
    }
    else
    {
        std::string msg_info("It is not possible to snapshot the negato view. This view is not shown on screen.");
        sight::ui::dialog::message message_box;
        message_box.set_title("Negato view snapshot");
        message_box.set_message(msg_info);
        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();
    }
}

//------------------------------------------------------------------------------

std::string snapshot_editor::request_file_name()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    std::string file_name;

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title("Save snapshot as");
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");
    dialog_file.add_filter("jpeg", "*.jpg *.jpeg");
    dialog_file.add_filter("bmp", "*.bmp");
    dialog_file.add_filter("png", "*.png");
    dialog_file.add_filter("tiff", "*.tiff");
    dialog_file.add_filter("all", "*.*");
    dialog_file.set_option(sight::ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        file_name = result->get_file().string();
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
    }

    return file_name;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
