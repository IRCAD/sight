/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/system.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <boost/algorithm/string.hpp>

// cspell:ignore sreader xmlattr NVJPEG LIBJPEG OPENJPEG

namespace sight::module::io::bitmap
{

// Retrieve the backend from the extension
sight::io::bitmap::backend reader::find_backend(const std::string& _extension) const
{
    const auto& it = std::find_if(
        m_backends.cbegin(),
        m_backends.cend(),
        [&](const auto& _backend)
        {
            const auto& backend_extensions = sight::io::bitmap::extensions(_backend);

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
        it == m_backends.cend()
    );

    // Return the backend if found...
    return *it;
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t reader::get_path_type() const
{
    return sight::io::service::path_type_t::file;
}

//------------------------------------------------------------------------------

void reader::open_location_dialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location location_dialog;

    // Set window title
    if(!m_window_title.empty())
    {
        location_dialog.set_title(m_window_title);
    }
    else
    {
        location_dialog.set_title("Enter file name");
    }

    location_dialog.set_default_location(default_location);
    location_dialog.set_option(ui::dialog::location::read);
    location_dialog.set_type(ui::dialog::location::single_file);

    // Will be used later to check if "All supported images" is selected
    std::string all_wildcards;

    // If there is at least two backends, add "all" filter
    if(m_backends.size() >= 2)
    {
        for(const auto& backend : m_backends)
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
    for(const auto& backend : m_backends)
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

void reader::starting()
{
}

//------------------------------------------------------------------------------

void reader::stopping()
{
}

//------------------------------------------------------------------------------

void reader::configuring()
{
    sight::io::service::reader::configuring();

    if(has_location_defined())
    {
        m_selected_backend = sight::io::bitmap::backend::any;
    }

    const auto& tree = get_config();

    // Dialog configuration
    if(const auto& dialog_tree = tree.get_child_optional("dialog.<xmlattr>"); dialog_tree.is_initialized())
    {
        m_dialog_policy = string_to_dialog_policy(dialog_tree->get<std::string>("policy", "default"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_dialog_policy == dialog_policy::invalid
        );
    }

    // Backend configuration
    if(const auto& backends_tree = tree.get_child_optional("backends"); backends_tree.is_initialized())
    {
        // Remove default configuration
        m_backends.clear();

        if(const auto& enabled = backends_tree->get_optional<std::string>("<xmlattr>.enable");
           enabled.is_initialized() && *enabled == "all")
        {
            // We add everything. Use GPU backend if available
            m_backends.emplace(sight::io::bitmap::backend::libpng);
            m_backends.emplace(sight::io::bitmap::backend::libtiff);

#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nv_jpeg())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg);
            }
            else
 #endif
            {
                m_backends.emplace(sight::io::bitmap::backend::libjpeg);
            }

#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nv_jpeg_2k())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg2k);
            }
            else
 #endif
            {
                m_backends.emplace(sight::io::bitmap::backend::openjpeg);
            }
        }
        else if(enabled.is_initialized() && *enabled == "cpu")
        {
            // We add only cpu backends
            m_backends.emplace(sight::io::bitmap::backend::libjpeg);
            m_backends.emplace(sight::io::bitmap::backend::libpng);
            m_backends.emplace(sight::io::bitmap::backend::libtiff);
            m_backends.emplace(sight::io::bitmap::backend::openjpeg);
        }
        else if(enabled.is_initialized() && *enabled == "gpu")
        {
            // We add only gpu backends, if possible
#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nv_jpeg())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg);
            }
#endif

#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nv_jpeg_2k())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg2k);
            }
#endif

            SIGHT_THROW_IF(
                "No GPU backend available.",
                !sight::io::bitmap::nv_jpeg() && !sight::io::bitmap::nv_jpeg_2k()
            );
        }

        // Add hand selected backends
        if(const auto& node = backends_tree->get_child_optional("default"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::backend::libtiff);
        }

        if(const auto& node = backends_tree->get_child_optional("libjpeg");
           node.is_initialized()
#if defined(SIGHT_ENABLE_NVJPEG)
           && !m_backends.contains(sight::io::bitmap::backend::nvjpeg)
#endif
        )
        {
            m_backends.emplace(sight::io::bitmap::backend::libjpeg);
        }

        if(const auto& node = backends_tree->get_child_optional("libpng"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::backend::libpng);
        }

        if(const auto& node = backends_tree->get_child_optional("libtiff"); node.is_initialized())
        {
            m_backends.emplace(sight::io::bitmap::backend::libtiff);
        }

        if(const auto& node = backends_tree->get_child_optional("openjpeg");
           node.is_initialized()
#if defined(SIGHT_ENABLE_NVJPEG2K)
           && !m_backends.contains(sight::io::bitmap::backend::nvjpeg2k)
#endif
        )
        {
            m_backends.emplace(sight::io::bitmap::backend::openjpeg);
        }

        if(const auto& node = backends_tree->get_child_optional("nvjpeg"); node.is_initialized())
        {
#if defined(SIGHT_ENABLE_NVJPEG)
            if(sight::io::bitmap::nv_jpeg())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg);

                // Remove libjpeg since we have a better alternative...
                m_backends.erase(sight::io::bitmap::backend::libjpeg);
            }
            else
#endif
            {
                SIGHT_WARN("nvjpeg GPU backend is not available. It will be replaced by libjpeg.");

                if(!m_backends.contains(sight::io::bitmap::backend::libjpeg))
                {
                    m_backends.emplace(sight::io::bitmap::backend::libjpeg);
                }
            }
        }

        if(const auto& node = backends_tree->get_child_optional("nvjpeg2k"); node.is_initialized())
        {
#if defined(SIGHT_ENABLE_NVJPEG2K)
            if(sight::io::bitmap::nv_jpeg_2k())
            {
                m_backends.emplace(sight::io::bitmap::backend::nvjpeg2k);

                // Remove openjpeg since we have a better alternative...
                m_backends.erase(sight::io::bitmap::backend::openjpeg);
            }
            else
#endif
            {
                SIGHT_WARN("nvjpeg2k GPU backend is not available. It will be replaced by openJPEG.");

                if(!m_backends.contains(sight::io::bitmap::backend::openjpeg))
                {
                    m_backends.emplace(sight::io::bitmap::backend::openjpeg);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void reader::updating()
{
    // Set to failed until successful
    m_read_failed = true;

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
            m_selected_backend = *std::next(m_backends.begin());
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
            + "' and will be read as '"
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
    }

    SIGHT_THROW_IF("The file '" << file_path << "' is an existing folder.", std::filesystem::is_directory(file_path));

    const auto write_job = std::make_shared<core::jobs::job>(
        "Writing '" + file_path.string() + "' file",
        [&](core::jobs::job& _running_job)
        {
            _running_job.done_work(10);

            // Create the session reader
            auto reader = std::make_shared<sight::io::bitmap::reader>();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                reader->set_object(data.get_shared());
                reader->set_file(file_path);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::busy_cursor busy_cursor;

            // Read the file
            reader->read(m_selected_backend);

            _running_job.done();
        },
        this->worker()
    );

    core::jobs::aggregator::sptr jobs = std::make_shared<core::jobs::aggregator>(file_path.string() + " reader");
    jobs->add(write_job);
    jobs->set_cancelable(false);

    m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_read_failed = false;
    }
    catch(std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Bitmap reader failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Bitmap reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::warning
        );
    }

    m_dialog_shown = false;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::bitmap
