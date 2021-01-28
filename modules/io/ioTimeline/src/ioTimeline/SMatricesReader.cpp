/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "ioTimeline/SMatricesReader.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <services/macros.hpp>

#include <boost/tokenizer.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <filesystem>
#include <fstream>

namespace ioTimeline
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioTimeline::SMatricesReader, ::sight::data::MatrixTL)

static const services::IService::KeyType s_MATRIXTL = "matrixTL";

static const core::com::Slots::SlotKeyType s_START_READING = "startReading";
static const core::com::Slots::SlotKeyType s_STOP_READING  = "stopReading";
static const core::com::Slots::SlotKeyType s_PAUSE         = "pause";

static const core::com::Slots::SlotKeyType s_READ_NEXT     = "readNext";
static const core::com::Slots::SlotKeyType s_READ_PREVIOUS = "readPrevious";
static const core::com::Slots::SlotKeyType s_SET_STEP      = "setStep";

//------------------------------------------------------------------------------

SMatricesReader::SMatricesReader() noexcept :
    m_isPlaying(false),
    m_filestream(nullptr),
    m_tsMatricesCount(0),
    m_createNewTS(false),
    m_fps(30),
    m_oneShot(false),
    m_useTimelapse(false),
    m_step(1),
    m_stepChanged(1)
{
    newSlot(s_START_READING, &SMatricesReader::startReading, this);
    newSlot(s_STOP_READING, &SMatricesReader::stopReading, this);
    newSlot(s_PAUSE, &SMatricesReader::pause, this);

    newSlot(s_READ_NEXT, &SMatricesReader::readNext, this);
    newSlot(s_READ_PREVIOUS, &SMatricesReader::readPrevious, this);
    newSlot(s_SET_STEP, &SMatricesReader::setStep, this);
}

//------------------------------------------------------------------------------

SMatricesReader::~SMatricesReader() noexcept
{
    if(nullptr != m_filestream)
    {
        m_filestream->close();
        delete m_filestream;
    }
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SMatricesReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SMatricesReader::configuring()
{
    ::fwIO::IReader::configuring();

    services::IService::ConfigType config = this->getConfigTree();

    m_fps = config.get<unsigned int>("fps", 30);
    SLM_ASSERT("Fps setting is set to " << m_fps << " but should be > 0.", m_fps > 0);

    m_useTimelapse = config.get<bool>("useTimelapse", m_useTimelapse);

    m_createNewTS = config.get<bool>("createTimestamp", m_createNewTS);

    m_oneShot = config.get<bool>("oneShot", m_oneShot);

    m_step = config.get<unsigned long>("step", m_step);
    SLM_ASSERT("Step value is set to " << m_step << " but should be > 0.", m_step > 0);
    m_stepChanged = m_step;
}

//------------------------------------------------------------------------------

void SMatricesReader::starting()
{
    m_worker = core::thread::Worker::New();
}

//------------------------------------------------------------------------------

void SMatricesReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SMatricesReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");
    ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a csv file to read" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());

        if(nullptr != m_filestream)
        {
            m_filestream->close();
        }

        m_filestream = new std::ifstream(this->getFile().string());
    }
    else
    {
        this->clearLocations();
    }

}

//------------------------------------------------------------------------------

void SMatricesReader::stopping()
{
    this->stopReading();
    m_worker->stop();
}

//------------------------------------------------------------------------------

void SMatricesReader::updating()
{
}

//------------------------------------------------------------------------------

void SMatricesReader::readPrevious()
{
    if(m_oneShot)
    {
        if(m_tsMatricesCount- m_step >= m_stepChanged)
        {
            // Compute difference between a possible step change in setStep() slot and the current step value
            const long shift = static_cast<long>(m_stepChanged - m_step);

            m_tsMatricesCount = m_tsMatricesCount - (2*m_step) - shift;// m_tsMatricesCount is pointing to previous
                                                                       // matrix,so -1 = present matrix
            m_step = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            ui::base::dialog::MessageDialog::show(
                "MatricesReader", "No previous Matrices.");
        }

    }
}

//------------------------------------------------------------------------------

void SMatricesReader::readNext()
{
    if(m_oneShot)
    {
        // Compute difference between a possible step change in setStep() slot and the current step value
        const long shift = static_cast<long>(m_stepChanged - m_step);

        if(m_tsMatricesCount + shift < m_tsMatrices.size())
        {
            // Update matrix position index
            m_tsMatricesCount += shift;
            m_step             = m_stepChanged;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            ui::base::dialog::MessageDialog::show(
                "MatricesReader", "No more matrices to read.");
        }
    }
}

//------------------------------------------------------------------------------

void SMatricesReader::setStep(int _step, std::string _key)
{
    if(_key == "step")
    {
        SLM_ASSERT("Needed step value (" << _step << ") should be > 0.", _step > 0);
        // Save the changed step value
        m_stepChanged = static_cast<unsigned long>(_step);
    }
    else
    {
        SLM_WARN("Only 'step' key is supported (current key value is : '" << _key << "').");
    }
}

//------------------------------------------------------------------------------

void SMatricesReader::startReading()
{
    if (m_timer)
    {
        this->stopReading();
    }

    if (!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if (this->hasLocationDefined())
    {
        if(nullptr == m_filestream)
        {
            std::string file = this->getFile().string();

            m_filestream = new std::ifstream(file);
        }

        if(m_filestream->is_open())
        {
            std::string line;
            while(std::getline(*m_filestream, line))
            {
                // parse the cvs file with tokenizer
                const ::boost::char_separator<char> sep(", ;");
                const ::boost::tokenizer< ::boost::char_separator<char> > tok {line, sep};

                // nb of 4x4 matrices = nb of elements - 1 (timestamp) / 16.
                const long int nbOfElements = std::distance(tok.begin(), tok.end());
                if(nbOfElements < 17)
                {
                    SLM_WARN("Too few elements("<<nbOfElements<< ") to convert this csv line into matrices");
                    continue;
                }
                const unsigned int nbOfMatrices = static_cast< unsigned int>((nbOfElements - 1 )/ 16);

                data::MatrixTL::sptr matrixTL = this->getInOut< data::MatrixTL>(s_MATRIXTL);
                matrixTL->initPoolSize(nbOfMatrices);

                TimeStampedMatrices currentTsMat;

                ::boost::tokenizer< ::boost::char_separator<char> >::iterator iter = tok.begin();
                currentTsMat.timestamp                                             = std::stod(iter.current_token());

                ++iter;

                for(unsigned int m = 0; m < nbOfMatrices; ++m)
                {
                    std::array< float, 16 > mat;
                    for(unsigned int i = 0; i < mat.size(); ++i)
                    {
                        mat[i] = std::stof(iter.current_token());

                        if(iter != tok.end())
                        {
                            ++iter;
                        }
                    }
                    currentTsMat.matrices.push_back(mat);
                }

                m_tsMatrices.push_back(currentTsMat);
            }

            m_filestream->close();
        }
        else
        {
            SLM_ERROR("The csv file '" + this->getFile().string() +"' can not be openned.");
        }

        if(m_oneShot)
        {
            m_timer = m_worker->createTimer();
            m_timer->setOneShot(true);
            m_timer->setFunction(std::bind(&SMatricesReader::readMatrices, this));
            m_timer->setDuration(std::chrono::milliseconds(0));
            m_timer->start();
        }
        else
        {
            m_timer = m_worker->createTimer();

            core::thread::Timer::TimeDurationType duration;
            if(m_useTimelapse)
            {
                m_timer->setOneShot(true);
                if(m_tsMatrices.size() >= 2)
                {
                    duration =
                        std::chrono::milliseconds(static_cast<std::uint64_t>(m_tsMatrices[1].timestamp -
                                                                             m_tsMatrices[0].timestamp));
                }
                else
                {
                    // Only one matrix to read, might as well just read it ASAP.
                    duration = std::chrono::milliseconds(0);
                }
            }
            else
            {
                duration = std::chrono::milliseconds(1000/m_fps);
            }

            m_timer->setFunction(std::bind(&SMatricesReader::readMatrices, this));
            m_timer->setDuration(duration);
            m_timer->start();
        }

        m_isPlaying = true;
    }
}

//------------------------------------------------------------------------------

void SMatricesReader::stopReading()
{
    m_isPlaying = false;

    if (m_timer)
    {
        if (m_timer->isRunning())
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
    auto matrixTL = this->getLockedInOut< data::MatrixTL>(s_MATRIXTL);
    matrixTL->clearTimeline();

    auto sig = matrixTL->signal< data::TimeLine::ObjectClearedSignalType >(data::TimeLine::s_CLEARED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SMatricesReader::pause()
{
    if (m_timer)
    {
        m_timer->isRunning() ? m_timer->stop() : m_timer->start();
    }
}

//------------------------------------------------------------------------------

void SMatricesReader::readMatrices()
{
    if(m_tsMatricesCount < m_tsMatrices.size())
    {
        const auto tStart             = core::HiResClock::getTimeInMilliSec();
        data::MatrixTL::sptr matrixTL = this->getInOut< data::MatrixTL>(s_MATRIXTL);

        TimeStampedMatrices currentMatrices = m_tsMatrices[m_tsMatricesCount];

        core::HiResClock::HiResClockType timestamp;

        if(m_createNewTS)
        {
            timestamp = core::HiResClock::getTimeInMilliSec();
        }
        else
        {
            timestamp = currentMatrices.timestamp;
        }

        // Push matrix in timeline
        SPTR(data::MatrixTL::BufferType) matrixBuf;
        matrixBuf = matrixTL->createBuffer(timestamp);
        matrixTL->pushObject(matrixBuf);

        SLM_DEBUG("Reading matrix index " << m_tsMatricesCount << " with timestamp " << timestamp);
        for(unsigned int i = 0; i < currentMatrices.matrices.size(); ++i)
        {
            float mat[16];
            std::copy(currentMatrices.matrices[i].begin(), currentMatrices.matrices[i].end(), &mat[0]);
            matrixBuf->setElement(mat, i);
        }

        if(m_useTimelapse && (m_tsMatricesCount + m_step) < m_tsMatrices.size())
        {
            const auto elapsedTime          = core::HiResClock::getTimeInMilliSec() - tStart;
            const std::size_t currentMatrix = m_tsMatricesCount;
            const double currentTime        = m_tsMatrices[currentMatrix].timestamp + elapsedTime;
            double nextDuration             = m_tsMatrices[m_tsMatricesCount + m_step].timestamp - currentTime;

            // If the next matrix delay is already passed, drop the matrices and check the next one.
            while (nextDuration < elapsedTime && (m_tsMatricesCount + m_step) < m_tsMatrices.size())
            {
                nextDuration       = m_tsMatrices[m_tsMatricesCount + m_step].timestamp - currentTime;
                m_tsMatricesCount += m_step;
                SLM_DEBUG("Skipping a matrix");
            }

            // If it is the last matrix array: stop the timer or loop
            if ( (m_tsMatricesCount + m_step) == m_tsMatrices.size())
            {
                m_timer->stop();
            }
            else
            {
                nextDuration = m_tsMatrices[m_tsMatricesCount + m_step].timestamp -
                               currentTime;
                core::thread::Timer::TimeDurationType duration =
                    std::chrono::milliseconds(static_cast<std::int64_t>(nextDuration));
                m_timer->stop();
                m_timer->setDuration(duration);
                m_timer->start();
            }
        }

        //Notify
        data::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = matrixTL->signal< data::TimeLine::ObjectPushedSignalType >(
            data::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);

        m_tsMatricesCount += m_step;

    }
}

//------------------------------------------------------------------------------

} // namespace ioTimeline
