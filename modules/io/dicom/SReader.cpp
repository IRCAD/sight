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

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/SeriesSet.hpp>

#include <io/dicom/Reader.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
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
                ss << series->getFile(i).string();
                ss << ", ";
            }

            ss << series->getFile(series->numInstances() - 1).string();
        }
        else
        {
            ss << series->getFile().string();
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
        m_job_created_signal(reader->newSignal<JobCreatedSignal>("jobCreated"))
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
        static auto default_location = std::make_shared<core::location::SingleFolder>();

        if(m_dialog_policy == DialogPolicy::ALWAYS
           || (m_dialog_policy == DialogPolicy::ONCE && !m_owner->hasLocationDefined()))
        {
            sight::ui::base::dialog::LocationDialog location_dialog;

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
            location_dialog.setOption(ui::base::dialog::ILocationDialog::READ);
            location_dialog.setType(ui::base::dialog::ILocationDialog::FOLDER);

            // Show the dialog
            const auto& selected_location = std::dynamic_pointer_cast<core::location::SingleFolder>(
                location_dialog.show()
            );

            // Exit if user canceled the dialog
            if(!selected_location)
            {
                return false;
            }

            // Set the selected location
            const auto& selected_folder = selected_location->getFolder();
            m_owner->setFolder(selected_folder);

            // Save default location for later use
            default_location->setFolder(selected_folder.parent_path());
            location_dialog.saveDefaultLocation(default_location);
        }

        return true;
    }

    //------------------------------------------------------------------------------

    inline bool scan()
    {
        // Set cursor to busy state. It will be reset to default even if exception occurs
        const sight::ui::base::BusyCursor busy_cursor;

        // Create the reader
        m_reader = sight::io::dicom::Reader::New();

        // Set the folder from the service location
        m_reader->setFolder(m_owner->getFolder());

        m_reader->setFilter(m_filter);

        // Scan the folder
        m_selection = m_reader->scan();

        // Exit if there is no DICOM files.
        if(!m_selection || m_selection->empty())
        {
            sight::ui::base::dialog::MessageDialog::show(
                "DICOM reader",
                "No DICOM files found in the selected folder.",
                sight::ui::base::dialog::IMessageDialog::WARNING
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
            const auto result = core::thread::getDefaultWorker()->postTask<std::pair<bool, data::SeriesSet::sptr> >(
                [selection]
                {
                    sight::ui::qt::series::SelectorDialog selector(selection);
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

    data::Series::DicomTypes m_filter {
        static_cast<data::Series::DicomTypes>(data::Series::DicomType::IMAGE)
        | static_cast<data::Series::DicomTypes>(data::Series::DicomType::MODEL)
        | static_cast<data::Series::DicomTypes>(data::Series::DicomType::REPORT)
    };

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
    sight::io::base::service::IReader::configuring();

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

        m_pimpl->m_filter = data::Series::stringToDicomTypes(dialog->get<std::string>("filter", "image,model,report"));
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

    auto jobs = core::jobs::Aggregator::New("DICOM reader");

    const auto sort_job = core::jobs::Job::New(
        "Sorting selected series",
        [&](core::jobs::Job& job)
        {
            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busy_cursor;

            job.doneWork(10);

            SIGHT_THROW_IF("No series were selected.", !m_pimpl->m_selection || m_pimpl->m_selection->empty());

            m_pimpl->m_reader->sort();

            job.done();
        },
        this->worker()
    );

    jobs->add(sort_job);

    const auto read_job = core::jobs::Job::New(
        "Reading sorted series",
        [&](core::jobs::Job& job)
        {
            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busy_cursor;
            SIGHT_THROW_IF("No series were selected.", !m_pimpl->m_selection || m_pimpl->m_selection->empty());

            // Really read the series
            m_pimpl->m_reader->read();

            job.done();
        },
        this->worker()
    );

    // Give the reader access to the job
    m_pimpl->m_reader->setJob(read_job);

    jobs->add(read_job);

    const auto build_job = core::jobs::Job::New(
        "Building series set from DICOM data",
        [&](core::jobs::Job& job)
        {
            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busy_cursor;

            job.doneWork(10);

            // Get the series set from the reader
            if(const auto& read = m_pimpl->m_reader->getConcreteObject(); read != nullptr && !read->empty())
            {
                // Retrieve data associated with this service
                const auto data   = m_data.lock();
                const auto output = data::SeriesSet::dynamicCast(data.get_shared());
                SIGHT_ASSERT("Output SeriesSet not instantiated", output);

                // Clear SeriesSet and add new series
                const auto scoped_emitter = output->scoped_emit();

                output->clear();
                output->shallowCopy(read);
            }

            job.done();
        },
        this->worker()
    );

    jobs->add(build_job);

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
        sight::ui::base::dialog::MessageDialog::show(
            "DICOM reader failed",
            e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::base::dialog::MessageDialog::show(
            "DICOM reader aborted",
            "Reading process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
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
        sight::ui::base::dialog::MessageDialog::show(
            "DICOM scanner",
            "Something wrong happened: " + std::string(e.what()),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::base::dialog::MessageDialog::show(
            "DICOM scanner aborted",
            "Scanning process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }

    // Something went wrong or user canceled, reset everything
    m_pimpl->clear();
}

} // namespace sight::module::io::dicom
