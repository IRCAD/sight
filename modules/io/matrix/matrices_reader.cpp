/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "matrices_reader.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <boost/tokenizer.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>

namespace sight::module::io::matrix
{

static const core::com::slots::key_t START_READING    = "startReading";
static const core::com::slots::key_t STOP_READING     = "stopReading";
static const core::com::slots::key_t PAUSE            = "pause";
static const core::com::slots::key_t TOGGLE_LOOP_MODE = "toggleLoopMode";

static const core::com::slots::key_t READ_NEXT     = "readNext";
static const core::com::slots::key_t READ_PREVIOUS = "readPrevious";
static const core::com::slots::key_t SET_STEP      = "setStep";

//------------------------------------------------------------------------------

matrices_reader::matrices_reader() noexcept
{
    new_slot(START_READING, &matrices_reader::startReading, this);
    new_slot(STOP_READING, &matrices_reader::stopReading, this);
    new_slot(PAUSE, &matrices_reader::pause, this);
    new_slot(TOGGLE_LOOP_MODE, &matrices_reader::toggleLoopMode, this);

    new_slot(READ_NEXT, &matrices_reader::readNext, this);
    new_slot(READ_PREVIOUS, &matrices_reader::readPrevious, this);
    new_slot(SET_STEP, &matrices_reader::setStep, this);
}

//------------------------------------------------------------------------------

matrices_reader::~matrices_reader() noexcept
{
    if(nullptr != m_filestream)
    {
        m_filestream->close();
        delete m_filestream;
    }
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType matrices_reader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void matrices_reader::configuring()
{
    sight::io::service::reader::configuring();

    service::config_t config = this->get_config();

    m_fps = config.get<unsigned int>("fps", 30);
    SIGHT_ASSERT("Fps setting is set to " << m_fps << " but should be > 0.", m_fps > 0);

    m_useTimelapse = config.get<bool>("useTimelapse", m_useTimelapse);

    m_createNewTS = config.get<bool>("createTimestamp", m_createNewTS);

    m_oneShot = config.get<bool>("oneShot", m_oneShot);

    m_step = config.get<std::uint64_t>("step", m_step);
    SIGHT_ASSERT("Step value is set to " << m_step << " but should be > 0.", m_step > 0);
    m_stepChanged = m_step;

    m_loopMatrix = config.get<bool>("loop", m_loopMatrix);
}

//------------------------------------------------------------------------------

void matrices_reader::starting()
{
    m_worker = core::thread::worker::make();
}

//------------------------------------------------------------------------------

void matrices_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a csv file to read" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setType(ui::dialog::location::SINGLE_FILE);
    dialog_file.addFilter(".csv file", "*.csv");

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
        this->set_file(result->get_file());

        if(nullptr != m_filestream)
        {
            m_filestream->close();
        }

        m_filestream = new std::ifstream(this->get_file().string());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void matrices_reader::stopping()
{
    this->stopReading();
    m_worker->stop();
}

//------------------------------------------------------------------------------

void matrices_reader::updating()
{
}

//------------------------------------------------------------------------------

void matrices_reader::readPrevious()
{
    if(m_oneShot)
    {
        if(m_tsMatricesCount - m_step >= m_stepChanged)
        {
            // Compute difference between a possible step change in setStep() slot and the current step value
            const std::int64_t shift   = static_cast<std::int64_t>(m_stepChanged) - static_cast<std::int64_t>(m_step);
            const std::int64_t shifted = static_cast<std::int64_t>(m_tsMatricesCount) - shift;

            // m_tsMatricesCount is pointing to previous matrix,so -1 = present matrix
            m_tsMatricesCount = static_cast<std::size_t>(shifted) - (2 * m_step);
            m_step            = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::dialog::message::show(
                "MatricesReader",
                "No previous Matrices."
            );
        }
    }
}

//------------------------------------------------------------------------------

void matrices_reader::readNext()
{
    if(m_oneShot)
    {
        // Compute difference between a possible step change in setStep() slot and the current step value
        const std::int64_t shift   = static_cast<std::int64_t>(m_stepChanged) - static_cast<std::int64_t>(m_step);
        const std::int64_t shifted = static_cast<std::int64_t>(m_tsMatricesCount) + shift;

        if(shifted < static_cast<std::int64_t>(m_tsMatrices.size()))
        {
            // Update matrix position index
            m_tsMatricesCount = static_cast<std::size_t>(shifted);
            m_step            = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::dialog::message::show(
                "MatricesReader",
                "No more matrices to read."
            );
        }
    }
}

//------------------------------------------------------------------------------

void matrices_reader::setStep(int _step, std::string _key)
{
    if(_key == "step")
    {
        SIGHT_ASSERT("Needed step value (" << _step << ") should be > 0.", _step > 0);

        // Save the changed step value
        m_stepChanged = static_cast<std::uint64_t>(_step);
    }
    else
    {
        SIGHT_WARN("Only 'step' key is supported (current key value is : '" << _key << "').");
    }
}

//------------------------------------------------------------------------------

void matrices_reader::startReading()
{
    if(m_timer)
    {
        this->stopReading();
    }

    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        if(nullptr == m_filestream)
        {
            std::string file = this->get_file().string();

            m_filestream = new std::ifstream(file);
        }

        if(m_filestream->is_open())
        {
            std::string line;
            while(std::getline(*m_filestream, line))
            {
                // parse the cvs file with tokenizer
                const boost::char_separator<char> sep(", ;");
                const boost::tokenizer<boost::char_separator<char> > tok {line, sep};

                // nb of 4x4 matrices = nb of elements - 1 (timestamp) / 16.
                const auto nb_of_elements = std::distance(tok.begin(), tok.end());
                if(nb_of_elements < 17)
                {
                    SIGHT_WARN("Too few elements(" << nb_of_elements << ") to convert this csv line into matrices");
                    continue;
                }

                const auto nb_of_matrices = static_cast<unsigned int>((nb_of_elements - 1) / 16);

                const auto matrix_tl = m_matrixTL.lock();
                matrix_tl->initPoolSize(nb_of_matrices);

                TimeStampedMatrices current_ts_mat;

                boost::tokenizer<boost::char_separator<char> >::iterator iter = tok.begin();
                current_ts_mat.timestamp = std::stod(iter.current_token());

                ++iter;

                for(unsigned int m = 0 ; m < nb_of_matrices ; ++m)
                {
                    std::array<float, 16> mat {};
                    for(float& i : mat)
                    {
                        i = std::stof(iter.current_token());

                        if(iter != tok.end())
                        {
                            ++iter;
                        }
                    }

                    current_ts_mat.matrices.push_back(mat);
                }

                m_tsMatrices.push_back(current_ts_mat);
            }

            m_filestream->close();
        }
        else
        {
            SIGHT_ERROR("The csv file '" + this->get_file().string() + "' can not be openned.");
        }

        if(m_oneShot)
        {
            m_timer = m_worker->create_timer();
            m_timer->set_one_shot(true);
            m_timer->set_function([this](auto&& ...){readMatrices();});
            m_timer->set_duration(std::chrono::milliseconds(0));
            m_timer->start();
        }
        else
        {
            m_timer = m_worker->create_timer();

            core::thread::timer::time_duration_t duration;
            if(m_useTimelapse)
            {
                m_timer->set_one_shot(true);
                if(m_tsMatrices.size() >= 2)
                {
                    duration =
                        std::chrono::milliseconds(
                            static_cast<std::uint64_t>(m_tsMatrices[1].timestamp
                                                       - m_tsMatrices[0].timestamp)
                        );
                }
                else
                {
                    // Only one matrix to read, might as well just read it ASAP.
                    duration = std::chrono::milliseconds(0);
                }
            }
            else
            {
                duration = std::chrono::milliseconds(1000 / m_fps);
            }

            m_timer->set_function([this](auto&& ...){readMatrices();});
            m_timer->set_duration(duration);
            m_timer->start();
        }

        m_isPlaying = true;
    }
}

//------------------------------------------------------------------------------

void matrices_reader::stopReading()
{
    m_isPlaying = false;

    if(m_timer)
    {
        if(m_timer->is_running())
        {
            m_timer->stop();
        }

        m_timer.reset();
    }

    if(nullptr != m_filestream)
    {
        m_filestream->close();
        delete m_filestream;
        m_filestream = nullptr;
    }

    if(!m_tsMatrices.empty())
    {
        m_tsMatrices.clear();
    }

    m_tsMatricesCount = 0;

    //clear the timeline
    const auto matrix_tl = m_matrixTL.lock();
    matrix_tl->clearTimeline();

    auto sig = matrix_tl->signal<data::timeline::signals::cleared_t>(data::timeline::signals::CLEARED);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void matrices_reader::pause()
{
    m_isPaused = !m_isPaused;
    if(m_timer)
    {
        m_isPaused ? m_timer->stop() : m_timer->start();
    }
}

//------------------------------------------------------------------------------

void matrices_reader::readMatrices()
{
    if(!m_isPaused && m_tsMatricesCount < m_tsMatrices.size())
    {
        const auto t_start   = core::hires_clock::get_time_in_milli_sec();
        const auto matrix_tl = m_matrixTL.lock();

        TimeStampedMatrices current_matrices = m_tsMatrices[m_tsMatricesCount];

        core::hires_clock::type timestamp = NAN;

        if(m_createNewTS)
        {
            timestamp = core::hires_clock::get_time_in_milli_sec();
        }
        else
        {
            timestamp = current_matrices.timestamp;
        }

        // Push matrix in timeline
        SPTR(data::matrix_tl::buffer_t) matrix_buf;
        matrix_buf = matrix_tl->createBuffer(timestamp);
        matrix_tl->pushObject(matrix_buf);

        SIGHT_DEBUG("Reading matrix index " << m_tsMatricesCount << " with timestamp " << timestamp);
        for(unsigned int i = 0 ; i < current_matrices.matrices.size() ; ++i)
        {
            matrix_buf->setElement(current_matrices.matrices[i], i);
        }

        if(m_useTimelapse && (m_tsMatricesCount + m_step) < m_tsMatrices.size())
        {
            const auto elapsed_time          = core::hires_clock::get_time_in_milli_sec() - t_start;
            const std::size_t current_matrix = m_tsMatricesCount;
            const double current_time        = m_tsMatrices[current_matrix].timestamp + elapsed_time;
            double next_duration             = m_tsMatrices[m_tsMatricesCount + m_step].timestamp - current_time;

            // If the next matrix delay is already passed, drop the matrices and check the next one.
            while(next_duration < elapsed_time && (m_tsMatricesCount + m_step) < m_tsMatrices.size())
            {
                next_duration      = m_tsMatrices[m_tsMatricesCount + m_step].timestamp - current_time;
                m_tsMatricesCount += m_step;
                SIGHT_DEBUG("Skipping a matrix");
            }

            // If it is the last matrix array: stop the timer or loop
            if((m_tsMatricesCount + m_step) == m_tsMatrices.size())
            {
                m_timer->stop();
                if(m_loopMatrix)
                {
                    matrix_tl->clearTimeline();
                    m_tsMatricesCount = 0;
                    core::thread::timer::time_duration_t duration = std::chrono::milliseconds(1000 / m_fps);
                    m_timer->set_duration(duration);
                    m_timer->start();
                }
            }
            else
            {
                next_duration = m_tsMatrices[m_tsMatricesCount + m_step].timestamp
                                - current_time;
                core::thread::timer::time_duration_t duration =
                    std::chrono::milliseconds(static_cast<std::int64_t>(next_duration));
                m_timer->stop();
                m_timer->set_duration(duration);
                m_timer->start();
            }
        }

        //Notify
        data::timeline::signals::pushed_t::sptr sig;
        sig = matrix_tl->signal<data::timeline::signals::pushed_t>(
            data::timeline::signals::PUSHED
        );
        sig->async_emit(timestamp);

        m_tsMatricesCount += m_step;
    }
    else if(!m_isPaused && m_loopMatrix)
    {
        const auto matrix_tl = m_matrixTL.lock();
        matrix_tl->clearTimeline();
        m_tsMatricesCount = 0;
    }
}

// -----------------------------------------------------------------------------

void matrices_reader::toggleLoopMode()
{
    m_loopMatrix = !m_loopMatrix;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::matrix
