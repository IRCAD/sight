/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "writer.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/system.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <boost/algorithm/string.hpp>

// cspell:ignore swriter xmlattr NVJPEG LIBJPEG OPENJPEG

namespace sight::module::io::bitmap
{

// Retrieve the backend from the extension
sight::io::bitmap::backend writer::find_backend(const std::string& _extension) const
{
    const auto& it = std::find_if(
        m_mode_by_backend.cbegin(),
        m_mode_by_backend.cend(),
        [&](const auto& _mode_by_backend)
        {
            const auto& backend_extensions = sight::io::bitmap::extensions(_mode_by_backend.first);

            return std::any_of(
                backend_extensions.cbegin(),
                backend_extensions.cend(),
                [&](const auto& _backend_extension)
            {
                return _extension.ends_with(_backend_extension);
            });
        });

    SIGHT_THROW_IF(
        "No backend found for file extension '" << _extension << "'.",
        it == m_mode_by_backend.cend()
    );

    // Return the backend if found...
    return it->first;
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t writer::get_path_type() const
{
    return sight::io::service::path_type_t::file;
}

//------------------------------------------------------------------------------

void writer::open_location_dialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location location_dialog;
    location_dialog.set_title(*m_window_title);
    location_dialog.set_default_location(default_location);
    location_dialog.set_option(ui::dialog::location::write);
    location_dialog.set_type(ui::dialog::location::single_file);

    // Will be used later to check if "All supported images" is selected
    std::string all_wildcards;

    // If there is at least two backends, add "all" filter
    if(m_mode_by_backend.size() >= 2)
    {
        for(const auto& [backend, mode] : m_mode_by_backend)
        {
            all_wildcards.append(sight::io::bitmap::wildcard_filter(backend).second);
            all_wildcards.append(" ");
        }

        boost::trim(all_wildcards);

        if(!all_wildcards.empty())
        {
            location_dialog.add_filter("All supported images", all_wildcards);
        }
    }

    // Add other filters
    for(const auto& [backend, mode] : m_mode_by_backend)
    {
        const auto& [label, wildcard] = sight::io::bitmap::wildcard_filter(backend);
        location_dialog.add_filter(label, wildcard);
    }

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::single_file>(location_dialog.show());

    if(result)
    {
        const auto& file_path = result->get_file();
        set_file(file_path);

        // Get the selected filter
        const auto& current_selection = boost::trim_copy(location_dialog.get_current_selection());

        // If "All supported images" is selected, try to guess the backend from the file extension
        if(!all_wildcards.empty() && all_wildcards == current_selection)
        {
            // Use file extension to guess the backend later
            m_selected_backend = sight::io::bitmap::backend::any;
        }
        else
        {
            try
            {
                // Find backend from selected filter
                m_selected_backend = find_backend(location_dialog.get_selected_extensions().front());
            }
            catch(...)
            {
                // Should normally not happen, but just in case, use file extension as fallback
                m_selected_backend = sight::io::bitmap::backend::any;
            }
        }

        // Save default location for later use
        default_location->set_folder(file_path.parent_path());
        location_dialog.save_default_location(default_location);

        m_dialog_shown = true;
    }
    else
    {
        clear_locations();
    }
}

//------------------------------------------------------------------------------

void writer::starting()
{
}

//------------------------------------------------------------------------------

void writer::stopping()
{
}

//------------------------------------------------------------------------------

void writer::configuring()
{
    sight::io::service::writer::configuring();

    if(has_location_defined())
    {
        m_selected_backend = sight::io::bitmap::backend::any;
    }

    const auto& config = get_config();

    // Dialog configuration
    if(const auto& dialog_tree = config.get_child_optional("dialog.<xmlattr>"); dialog_tree.is_initialized())
    {
        m_dialog_policy = string_to_dialog_policy(dialog_tree->get<std::string>("policy", "default"));

        SIGHT_THROW_IF("Cannot read dialog policy.", m_dialog_policy == dialog_policy::invalid);
    }

    [[maybe_unused]] const bool gpu_required = config.get("gpu_required", false);

    // Lambda to parse "mode" string
    const auto& string_to_mode = [](const std::string& _mode_string)
                                 {
                                     if(_mode_string == "default" || _mode_string == "fast")
                                     {
                                         return sight::io::bitmap::writer::mode::fast;
                                     }

                                     if(_mode_string == "best")
                                     {
                                         return sight::io::bitmap::writer::mode::best;
                                     }

                                     SIGHT_THROW("Unknown mode: '" << _mode_string << "'.");
                                 };

    const auto mode = string_to_mode(config.get("mode", std::string("default")));

    // We add everything. Use GPU backend if available
    m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::libpng, mode);
    m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::libtiff, mode);

#if defined(SIGHT_ENABLE_NVJPEG)
    if(sight::io::bitmap::nv_jpeg())
    {
        m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::nvjpeg, mode);
    }
    else
#else
    if(gpu_required)
    {
        ui::dialog::message::show(
            "Error",
            "GPU support required to write jpeg bitmaps but it was not built.",
            ui::dialog::message::critical
        );
    }
#endif
    {
        m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::libjpeg, mode);
    }

#if defined(SIGHT_ENABLE_NVJPEG2K)
    if(sight::io::bitmap::nv_jpeg_2k())
    {
        m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::nvjpeg2k, mode);
    }
    else
#else
    if(gpu_required)
    {
        ui::dialog::message::show(
            "Error",
            "GPU support required to write jpeg2k bitmaps but it was not built.",
            ui::dialog::message::critical
        );
    }
#endif
    {
        m_mode_by_backend.insert_or_assign(sight::io::bitmap::backend::openjpeg, mode);
    }
}

//------------------------------------------------------------------------------

void writer::updating()
{
    // Set to failed until successful
    m_write_failed = true;

    // Show the save dialog if the path is empty
    if((!has_location_defined() && m_dialog_policy != dialog_policy::never)
       || (!m_dialog_shown && m_dialog_policy == dialog_policy::always))
    {
        open_location_dialog();
    }

    // If the user did not choose a file, we stop here
    if(!has_location_defined())
    {
        return;
    }

    // Check if we must add an extension or not
    auto file_path                = get_file();
    const auto& current_extension = file_path.extension().string();

    try
    {
        // Find the backend to use for the current file extension in the list of enabled backends
        const auto& backend_from_extension = find_backend(current_extension);

        // If we can use any backend, we shall use the one found
        if(m_selected_backend == sight::io::bitmap::backend::any)
        {
            m_selected_backend = backend_from_extension;
        }
        else if(m_selected_backend != backend_from_extension)
        {
            // If the user selected a specific backend, it must match the one given by the file extension.
            SIGHT_THROW(
                "Backend mismatch: "
                << std::uint8_t(m_selected_backend)
                << " != "
                << std::uint8_t(backend_from_extension)
            );
        }
    }
    catch(...)
    {
        // If there is no selected backend, use the first available one
        // This could happen if the extension is not supported by any enabled backends
        if(m_selected_backend == sight::io::bitmap::backend::any)
        {
            m_selected_backend = std::next(m_mode_by_backend.begin())->first;
        }

        // If we are there, it means that the file extension is not supported or there is no extension.
        // Warn the user and force the extension to be something valid
        const std::string& message =
            (
                current_extension.empty()
                ? "There is no filename extension set"
                : "The selected filename extension '" + current_extension + "' is not valid"
            )
            + " for the selected format '"
            + sight::io::bitmap::wildcard_filter(m_selected_backend).first
            + "' and will be adjusted to '"
            + sight::io::bitmap::extensions(m_selected_backend).front()
            + "'.";

        if(m_dialog_policy != dialog_policy::never || m_dialog_policy == dialog_policy::always)
        {
            ui::dialog::message critical_dialog;
            critical_dialog.set_icon(ui::dialog::message::critical);
            critical_dialog.set_title("Missing or Wrong file extension");
            critical_dialog.set_message(message + "\n\nWould you like to continue ?");
            critical_dialog.add_button(ui::dialog::message::ok);
            critical_dialog.add_button(ui::dialog::message::cancel);
            critical_dialog.set_default_button(ui::dialog::message::cancel);

            if(critical_dialog.show() == ui::dialog::message::cancel)
            {
                return;
            }
        }
        else
        {
            SIGHT_WARN(message);
        }

        // Use the correct extension
        file_path.replace_extension(sight::io::bitmap::extensions(m_selected_backend).front());
    }

    SIGHT_THROW_IF("The file '" << file_path << "' is an existing folder.", std::filesystem::is_directory(file_path));

    const auto write_job = std::make_shared<core::jobs::job>(
        "Writing '" + file_path.string() + "' file",
        [&](core::jobs::job& _running_job)
        {
            _running_job.done_work(10);

            // Create the session writer
            auto writer = std::make_shared<sight::io::bitmap::writer>();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                writer->set_object(data.get_shared());
                writer->set_file(file_path);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::busy_cursor busy_cursor;

            // Write the file
            writer->write(m_selected_backend, m_mode_by_backend.at(m_selected_backend));

            _running_job.done();
        },
        this->worker()
    );

    core::jobs::aggregator::sptr jobs = std::make_shared<core::jobs::aggregator>(file_path.string() + " writer");
    jobs->add(write_job);
    jobs->set_cancelable(false);

    m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_write_failed = false;
    }
    catch(std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Bitmap writer failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Bitmap writer aborted",
            "Writing process aborted",
            sight::ui::dialog::message::warning
        );
    }

    m_dialog_shown = false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap
