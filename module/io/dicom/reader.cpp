/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include "module/io/dicom/reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>

#include <data/series_set.hpp>

#include <io/dicom/reader/file.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/series/selector_dialog.hpp>

namespace sight::module::io::dicom
{

/// Debug function to print the files associated to series in a series_set
inline static auto print_series_set(const sight::data::series_set& _series_set)
{
    std::stringstream ss;

    for(const auto& series : _series_set)
    {
        ss << series->get_series_instance_uid() << " { ";

        if(series->num_instances() > 1)
        {
            for(std::size_t i = 0, end = series->num_instances() - 1 ; i < end ; ++i)
            {
                ss << series->get_file(i).string();
                ss << ", ";
            }

            ss << series->get_file(series->num_instances() - 1).string();
        }
        else
        {
            ss << series->get_file().string();
        }

        ss << " }" << std::endl;
    }

    return ss.str();
}

/// Private reader implementation
class reader::reader_impl
{
public:

    /// Delete default constructors and assignment operators
    reader_impl(const reader_impl&)            = delete;
    reader_impl(reader_impl&&)                 = delete;
    reader_impl& operator=(const reader_impl&) = delete;
    reader_impl& operator=(reader_impl&&)      = delete;

    /// Constructor
    explicit reader_impl(reader* const _reader) noexcept :
        m_owner(_reader),
        m_job_created_signal(_reader->new_signal<job_created_signal_t>("job_created"))
    {
    }

    /// Default destructor
    ~reader_impl() noexcept = default;

    /// Pointer to the public interface
    reader* const m_owner;

    /// Clear location and selected series
    void clear()
    {
        m_owner->clear_locations();
        m_reader.reset();
        m_selection.reset();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool show_location() const
    {
        static auto default_location = std::make_shared<core::location::single_folder>();

        if(m_dialog_policy == dialog_policy::always
           || (m_dialog_policy == dialog_policy::once && !m_owner->has_location_defined()))
        {
            sight::ui::dialog::location location_dialog;
            location_dialog.set_title(*m_owner->m_window_title);
            location_dialog.set_default_location(default_location);
            location_dialog.set_option(ui::dialog::location::read);
            location_dialog.set_type(ui::dialog::location::folder);

            // Show the dialog
            const auto& selected_location = std::dynamic_pointer_cast<core::location::single_folder>(
                location_dialog.show()
            );

            // Exit if user canceled the dialog
            if(!selected_location)
            {
                return false;
            }

            // Set the selected location
            const auto& selected_folder = selected_location->get_folder();
            m_owner->set_folder(selected_folder);

            // Save default location for later use
            default_location->set_folder(selected_folder.parent_path());
            location_dialog.save_default_location(default_location);
        }

        return true;
    }

    //------------------------------------------------------------------------------

    bool scan()
    {
        // Set cursor to busy state. It will be reset to default even if exception occurs
        const sight::ui::busy_cursor busy_cursor;

        // Create the reader
        m_reader = std::make_shared<sight::io::dicom::reader::file>();

        // Set the folder from the service location
        m_reader->set_folder(m_owner->get_folder());

        // Set filters
        m_reader->set_filters(m_filters);

        // Scan the folder
        m_selection = m_reader->scan();

        // Exit if there is no DICOM files.
        if(!m_selection || m_selection->empty())
        {
            sight::ui::dialog::message::show(
                "DICOM reader",
                "No DICOM files found in the selected folder.",
                sight::ui::dialog::message::warning
            );

            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------

    bool show_selection()
    {
        // if we found more than one series, let the user choose them
        if(m_selection->size() > 1)
        {
            // Make an explicit copy of the shared pointer, since the task is posted...
            const auto selection = m_selection;

            // Ask the user to select one or more series
            const auto result = core::thread::get_default_worker()->post_task<std::pair<bool, data::series_set::sptr> >(
                [this, selection]
                {
                    sight::ui::qt::series::selector_dialog selector(selection, m_displayed_columns);
                    if(selector.exec() != QDialog::Rejected)
                    {
                        return std::make_pair(true, selector.get_selection());
                    }

                    return std::make_pair(false, data::series_set::sptr());
                }).get();

            // Exit if user canceled the dialog
            if(!result.first)
            {
                return false;
            }

            // Keep only selected series
            m_selection = result.second;
            m_reader->set_scanned(m_selection);
        }

        return true;
    }

    /// Dialog policy to use for dialogs. By default, always show dialog
    dialog_policy m_dialog_policy {dialog_policy::always};

    /// Default filters to use when scanning for DICOM files
    data::series::SopKeywords m_filters {
        data::series::dicom_types_to_sops(
            static_cast<data::series::dicom_types>(data::series::dicom_t::image)
            | static_cast<data::series::dicom_types>(data::series::dicom_t::model)
            | static_cast<data::series::dicom_types>(data::series::dicom_t::report)
        )
    };

    std::string m_displayed_columns =
        "PatientName/SeriesInstanceUID,PatientSex,PatientBirthDate/Icon,Modality,StudyDescription/SeriesDescription,StudyDate/SeriesDate,StudyTime/SeriesTime,PatientAge,BodyPartExamined,PatientPositionString,ContrastBolusAgent,AcquisitionTime,ContrastBolusStartTime";

    /// Signal emitted when job created.
    job_created_signal_t::sptr m_job_created_signal;

    /// The reader to use to read all DICOM files
    sight::io::dicom::reader::file::sptr m_reader;

    /// This will hold the scanned / selected series
    sight::data::series_set::sptr m_selection;
};

reader::reader() noexcept :
    sight::io::service::reader("Enter DICOM directory name"),
    m_pimpl(std::make_unique<reader_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
reader::~reader() noexcept = default;

//-----------------------------------------------------------------------------

void reader::starting()
{
}

//-----------------------------------------------------------------------------

void reader::stopping()
{
    m_pimpl->clear();
}

//-----------------------------------------------------------------------------

void reader::configuring()
{
    sight::io::service::reader::configuring();

    const auto& tree = this->get_config();

    // Extension configuration
    const auto& dialog = tree.get_child_optional("dialog.<xmlattr>");
    if(dialog.is_initialized())
    {
        m_pimpl->m_dialog_policy = string_to_dialog_policy(dialog->get<std::string>("policy", "always"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_pimpl->m_dialog_policy == dialog_policy::invalid
        );

        m_pimpl->m_filters.clear();

        if(const auto sop_filter = dialog->get_optional<std::string>("sopFilter"); sop_filter)
        {
            const auto& sop_filters = data::series::string_to_sops(*sop_filter);
            m_pimpl->m_filters.insert(sop_filters.cbegin(), sop_filters.cend());
        }

        if(const auto type_filter = dialog->get_optional<std::string>("typeFilter"); type_filter)
        {
            const auto& type_filters = data::series::dicom_types_to_sops(
                data::series::string_to_dicom_types(*type_filter)
            );

            m_pimpl->m_filters.insert(type_filters.cbegin(), type_filters.cend());
        }

        // Compat
        if(const auto compat_filter = dialog->get_optional<std::string>("filter"); compat_filter)
        {
            const auto& compat_filters = data::series::dicom_types_to_sops(
                data::series::string_to_dicom_types(*compat_filter)
            );

            m_pimpl->m_filters.insert(compat_filters.cbegin(), compat_filters.cend());
        }

        // If no filters are defined, use the default ones (image, model and report)
        if(m_pimpl->m_filters.empty())
        {
            m_pimpl->m_filters = data::series::dicom_types_to_sops(
                static_cast<data::series::dicom_types>(data::series::dicom_t::image)
                | static_cast<data::series::dicom_types>(data::series::dicom_t::model)
                | static_cast<data::series::dicom_types>(data::series::dicom_t::report)
            );
        }
    }

    const auto& config = tree.get_child_optional("config.<xmlattr>");
    if(config.is_initialized())
    {
        if(std::string displayed_columns = config->get("displayedColumns", ""); !displayed_columns.empty())
        {
            m_pimpl->m_displayed_columns = displayed_columns;
        }
    }
}

//-----------------------------------------------------------------------------

void reader::updating()
{
    // Set to failed until successful
    m_read_failed = true;

    // If the user did not choose a series, we stop here
    if(!m_pimpl->m_reader)
    {
        return;
    }

    auto jobs = std::make_shared<core::jobs::aggregator>("DICOM reader");

    const auto read_job = std::make_shared<core::jobs::job>(
        "Sorting selected series",
        [&](core::jobs::job& _job)
        {
            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::busy_cursor busy_cursor;

            _job.done_work(10);

            SIGHT_THROW_IF("No series were selected.", !m_pimpl->m_selection || m_pimpl->m_selection->empty());

            // Sort the series
            m_pimpl->m_reader->sort();

            _job.done_work(20);

            // Really read the series
            m_pimpl->m_reader->read();

            _job.done_work(90);

            // Get the series set from the reader
            if(const auto& read = m_pimpl->m_reader->get_concrete_object(); read != nullptr && !read->empty())
            {
                // Retrieve data associated with this service
                const auto data   = m_data.lock();
                const auto output = std::dynamic_pointer_cast<data::series_set>(data.get_shared());
                SIGHT_ASSERT("Output series_set not instantiated", output);

                // Clear series_set and add new series
                const auto scoped_emitter = output->scoped_emit();

                output->clear();
                output->shallow_copy(read);
            }

            _job.done();
        },
        this->worker()
    );

    jobs->add(read_job);

    // Give the reader access to the job
    m_pimpl->m_reader->set_job(read_job);
    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_read_failed = false;
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "DICOM reader failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "DICOM reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::warning
        );
    }

    // Reset reader state...
    m_pimpl->clear();
}

//-----------------------------------------------------------------------------

void reader::open_location_dialog()
{
    try
    {
        // Show the location dialog, if needed
        if(m_pimpl->show_location()
           // Scan the selected folder
           && m_pimpl->scan()
           // Show the series selection dialog, if needed
           && m_pimpl->show_selection())
        {
            SIGHT_DEBUG("Selected series: " << print_series_set(*m_pimpl->m_selection));

            // Everything seems going well, exit
            return;
        }
    }
    catch(const std::exception& e)
    {
        sight::ui::dialog::message::show(
            "DICOM scanner",
            "Something wrong happened: " + std::string(e.what()),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "DICOM scanner aborted",
            "Scanning process aborted",
            sight::ui::dialog::message::warning
        );
    }

    // Something went wrong or user canceled, reset everything
    m_pimpl->clear();
}

} // namespace sight::module::io::dicom
