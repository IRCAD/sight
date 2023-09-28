/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "modules/io/dicom/SReader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>

#include <data/SeriesSet.hpp>

#include <io/dicom/Reader.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/series/SelectorDialog.hpp>

namespace sight::module::io::dicom
{

/// Debug function to print the files associated to series in a SeriesSet
inline static auto printSeriesSet(const sight::data::SeriesSet& series_set)
{
    std::stringstream ss;

    for(const auto& series : series_set)
    {
        ss << series->getSeriesInstanceUID() << " { ";

        if(series->numInstances() > 1)
        {
            for(std::size_t i = 0, end = series->numInstances() - 1 ; i < end ; ++i)
            {
                ss << series->get_file(i).string();
                ss << ", ";
            }

            ss << series->get_file(series->numInstances() - 1).string();
        }
        else
        {
            ss << series->get_file().string();
        }

        ss << " }" << std::endl;
    }

    return ss.str();
}

/// Private SReader implementation
class SReader::SReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    SReaderImpl(const SReaderImpl&)            = delete;
    SReaderImpl(SReaderImpl&&)                 = delete;
    SReaderImpl& operator=(const SReaderImpl&) = delete;
    SReaderImpl& operator=(SReaderImpl&&)      = delete;

    /// Constructor
    inline explicit SReaderImpl(SReader* const reader) noexcept :
        m_owner(reader),
        m_job_created_signal(reader->new_signal<JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~SReaderImpl() noexcept = default;

    /// Pointer to the public interface
    SReader* const m_owner;

    /// Clear location and selected series
    inline void clear()
    {
        m_owner->clearLocations();
        m_reader.reset();
        m_selection.reset();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool showLocation() const
    {
        static auto default_location = std::make_shared<core::location::single_folder>();

        if(m_dialog_policy == DialogPolicy::ALWAYS
           || (m_dialog_policy == DialogPolicy::ONCE && !m_owner->hasLocationDefined()))
        {
            sight::ui::dialog::location location_dialog;

            // Set dialog options
            if(!m_owner->m_windowTitle.empty())
            {
                location_dialog.setTitle(m_owner->m_windowTitle);
            }
            else
            {
                location_dialog.setTitle("Enter DICOM directory name");
            }

            location_dialog.setDefaultLocation(default_location);
            location_dialog.setOption(ui::dialog::location::READ);
            location_dialog.setType(ui::dialog::location::FOLDER);

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
            location_dialog.saveDefaultLocation(default_location);
        }

        return true;
    }

    //------------------------------------------------------------------------------

    inline bool scan()
    {
        // Set cursor to busy state. It will be reset to default even if exception occurs
        const sight::ui::BusyCursor busy_cursor;

        // Create the reader
        m_reader = std::make_shared<sight::io::dicom::Reader>();

        // Set the folder from the service location
        m_reader->set_folder(m_owner->get_folder());

        // Set filters
        m_reader->setFilters(m_filters);

        // Scan the folder
        m_selection = m_reader->scan();

        // Exit if there is no DICOM files.
        if(!m_selection || m_selection->empty())
        {
            sight::ui::dialog::message::show(
                "DICOM reader",
                "No DICOM files found in the selected folder.",
                sight::ui::dialog::message::WARNING
            );

            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------

    inline bool showSelection()
    {
        // if we found more than one series, let the user choose them
        if(m_selection->size() > 1)
        {
            // Make an explicit copy of the shared pointer, since the task is posted...
            const auto selection = m_selection;

            // Ask the user to select one or more series
            const auto result = core::thread::get_default_worker()->post_task<std::pair<bool, data::SeriesSet::sptr> >(
                [this, selection]
                {
                    sight::ui::qt::series::selector selector(selection, m_displayedColumns);
                    if(selector.exec() != QDialog::Rejected)
                    {
                        return std::make_pair(true, selector.get_selection());
                    }

                    return std::make_pair(false, data::SeriesSet::sptr());
                }).get();

            // Exit if user canceled the dialog
            if(!result.first)
            {
                return false;
            }

            // Keep only selected series
            m_selection = result.second;
            m_reader->setScanned(m_selection);
        }

        return true;
    }

    /// Dialog policy to use for dialogs. By default, always show dialog
    DialogPolicy m_dialog_policy {DialogPolicy::ALWAYS};

    /// Default filters to use when scanning for DICOM files
    data::Series::SopKeywords m_filters {
        data::Series::dicomTypesToSops(
            static_cast<data::Series::DicomTypes>(data::Series::DicomType::IMAGE)
            | static_cast<data::Series::DicomTypes>(data::Series::DicomType::MODEL)
            | static_cast<data::Series::DicomTypes>(data::Series::DicomType::REPORT)
        )
    };

    std::string m_displayedColumns =
        "PatientName/SeriesInstanceUID,PatientSex,PatientBirthDate/Icon,Modality,StudyDescription/SeriesDescription,StudyDate/SeriesDate,StudyTime/SeriesTime,PatientAge,BodyPartExamined,PatientPositionString,ContrastBolusAgent,AcquisitionTime,ContrastBolusStartTime";

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal;

    /// The reader to use to read all DICOM files
    sight::io::dicom::Reader::sptr m_reader;

    /// This will hold the scanned / selected series
    sight::data::SeriesSet::sptr m_selection;
};

SReader::SReader() noexcept :
    m_pimpl(std::make_unique<SReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SReader::~SReader() noexcept = default;

//-----------------------------------------------------------------------------

void SReader::starting()
{
}

//-----------------------------------------------------------------------------

void SReader::stopping()
{
    m_pimpl->clear();
}

//-----------------------------------------------------------------------------

void SReader::configuring()
{
    sight::io::service::reader::configuring();

    const auto& tree = this->getConfiguration();

    // Extension configuration
    const auto& dialog = tree.get_child_optional("dialog.<xmlattr>");
    if(dialog.is_initialized())
    {
        m_pimpl->m_dialog_policy = stringToDialogPolicy(dialog->get<std::string>("policy", "always"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_pimpl->m_dialog_policy == DialogPolicy::INVALID
        );

        m_pimpl->m_filters.clear();

        if(const auto sop_filter = dialog->get_optional<std::string>("sopFilter"); sop_filter)
        {
            const auto& sop_filters = data::Series::stringToSops(*sop_filter);
            m_pimpl->m_filters.insert(sop_filters.cbegin(), sop_filters.cend());
        }

        if(const auto type_filter = dialog->get_optional<std::string>("typeFilter"); type_filter)
        {
            const auto& type_filters = data::Series::dicomTypesToSops(
                data::Series::stringToDicomTypes(*type_filter)
            );

            m_pimpl->m_filters.insert(type_filters.cbegin(), type_filters.cend());
        }

        // Compat
        if(const auto compat_filter = dialog->get_optional<std::string>("filter"); compat_filter)
        {
            const auto& compat_filters = data::Series::dicomTypesToSops(
                data::Series::stringToDicomTypes(*compat_filter)
            );

            m_pimpl->m_filters.insert(compat_filters.cbegin(), compat_filters.cend());
        }

        // If no filters are defined, use the default ones (image, model and report)
        if(m_pimpl->m_filters.empty())
        {
            m_pimpl->m_filters = data::Series::dicomTypesToSops(
                static_cast<data::Series::DicomTypes>(data::Series::DicomType::IMAGE)
                | static_cast<data::Series::DicomTypes>(data::Series::DicomType::MODEL)
                | static_cast<data::Series::DicomTypes>(data::Series::DicomType::REPORT)
            );
        }
    }

    const auto& config = tree.get_child_optional("config.<xmlattr>");
    if(config.is_initialized())
    {
        if(std::string displayedColumns = config->get("displayedColumns", ""); !displayedColumns.empty())
        {
            m_pimpl->m_displayedColumns = displayedColumns;
        }
    }
}

//-----------------------------------------------------------------------------

void SReader::updating()
{
    // Set to failed until successful
    m_readFailed = true;

    // If the user did not choose a series, we stop here
    if(!m_pimpl->m_reader)
    {
        return;
    }

    auto jobs = std::make_shared<core::jobs::aggregator>("DICOM reader");

    const auto read_job = std::make_shared<core::jobs::job>(
        "Sorting selected series",
        [&](core::jobs::job& job)
        {
            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::BusyCursor busy_cursor;

            job.done_work(10);

            SIGHT_THROW_IF("No series were selected.", !m_pimpl->m_selection || m_pimpl->m_selection->empty());

            // Sort the series
            m_pimpl->m_reader->sort();

            job.done_work(20);

            // Really read the series
            m_pimpl->m_reader->read();

            job.done_work(90);

            // Get the series set from the reader
            if(const auto& read = m_pimpl->m_reader->getConcreteObject(); read != nullptr && !read->empty())
            {
                // Retrieve data associated with this service
                const auto data   = m_data.lock();
                const auto output = std::dynamic_pointer_cast<data::SeriesSet>(data.get_shared());
                SIGHT_ASSERT("Output SeriesSet not instantiated", output);

                // Clear SeriesSet and add new series
                const auto scoped_emitter = output->scoped_emit();

                output->clear();
                output->shallow_copy(read);
            }

            job.done();
        },
        this->worker()
    );

    jobs->add(read_job);

    // Give the reader access to the job
    m_pimpl->m_reader->setJob(read_job);
    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_readFailed = false;
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "DICOM reader failed",
            e.what(),
            sight::ui::dialog::message::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "DICOM reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::WARNING
        );
    }

    // Reset reader state...
    m_pimpl->clear();
}

//-----------------------------------------------------------------------------

void SReader::openLocationDialog()
{
    try
    {
        // Show the location dialog, if needed
        if(m_pimpl->showLocation()
           // Scan the selected folder
           && m_pimpl->scan()
           // Show the series selection dialog, if needed
           && m_pimpl->showSelection())
        {
            SIGHT_DEBUG("Selected series: " << printSeriesSet(*m_pimpl->m_selection));

            // Everything seems going well, exit
            return;
        }
    }
    catch(const std::exception& e)
    {
        sight::ui::dialog::message::show(
            "DICOM scanner",
            "Something wrong happened: " + std::string(e.what()),
            sight::ui::dialog::message::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "DICOM scanner aborted",
            "Scanning process aborted",
            sight::ui::dialog::message::WARNING
        );
    }

    // Something went wrong or user canceled, reset everything
    m_pimpl->clear();
}

} // namespace sight::module::io::dicom
