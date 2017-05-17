/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioTimeline/SMatricesReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <boost/chrono.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/tokenizer.hpp>

namespace ioTimeline
{

fwServicesRegisterMacro( ::io::IReader, ::ioTimeline::SMatricesReader, ::arData::MatrixTL);

static const ::fwServices::IService::KeyType s_MATRIXTL = "matrixTL";

static const ::fwCom::Slots::SlotKeyType s_START_READING = "startReading";
static const ::fwCom::Slots::SlotKeyType s_STOP_READING  = "stopReading";
static const ::fwCom::Slots::SlotKeyType s_PAUSE         = "pause";

static const ::fwCom::Slots::SlotKeyType s_READ_NEXT     = "readNext";
static const ::fwCom::Slots::SlotKeyType s_READ_PREVIOUS = "readPrevious";

//------------------------------------------------------------------------------

SMatricesReader::SMatricesReader() throw() :
    m_isPlaying(false),
    m_filestream(nullptr),
    m_tsMatricesCount(0),
    m_createNewTS(false),
    m_fps(30),
    m_oneShot(false)
{

    m_worker = ::fwThread::Worker::New();
    newSlot(s_START_READING, &SMatricesReader::startReading, this);
    newSlot(s_STOP_READING, &SMatricesReader::stopReading, this);
    newSlot(s_PAUSE, &SMatricesReader::pause, this);

    newSlot(s_READ_NEXT, &SMatricesReader::readNext, this);
    newSlot(s_READ_PREVIOUS, &SMatricesReader::readPrevious, this);
}

//------------------------------------------------------------------------------

SMatricesReader::~SMatricesReader() throw()
{
    if(nullptr != m_filestream)
    {
        m_filestream->close();
        delete m_filestream;
    }
}

//------------------------------------------------------------------------------

::io::IOPathType SMatricesReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SMatricesReader::configuring() throw(::fwTools::Failed)
{
    ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("service");

    m_fps = config.get<unsigned int>("fps", 30);
    OSLM_ASSERT("Fps setting is set to " << m_fps << " but should be > 0.", m_fps > 0);

    m_createNewTS = config.get<bool>("createTimestamp", false);

    m_oneShot = config.get<bool>("oneShot", false);
}

//------------------------------------------------------------------------------

void SMatricesReader::starting() throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SMatricesReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath("");
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a csv file to read");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter(".csv file", "*.csv");

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(_sDefaultPath);

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

void SMatricesReader::stopping() throw(::fwTools::Failed)
{
    this->stopReading();
}

//------------------------------------------------------------------------------

void SMatricesReader::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SMatricesReader::readPrevious()
{
    if(m_oneShot)
    {
        if(m_tsMatricesCount > 1)
        {
            m_tsMatricesCount = m_tsMatricesCount - 2;// m_tsMatricesCount is pointing to previous image,so -1 = present
                                                      // image
            m_timer->stop();
            m_timer->start();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "MatricesReader", "No previous Matrices.");
        }

    }
}

//------------------------------------------------------------------------------

void SMatricesReader::readNext()
{
    if(m_oneShot)
    {
        if(m_tsMatricesCount < m_tsMatrices.size())
        {
            m_timer->stop();
            m_timer->start();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "MatricesReader", "No more matrices to read.");
        }
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
        this->configureWithIHM();
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
                    OSLM_WARN("Too few elements("<<nbOfElements<< ") to convert this csv line into matrices");
                    continue;
                }
                const unsigned int nbOfMatrices = static_cast< unsigned int>((nbOfElements - 1 )/ 16);

                ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL>(s_MATRIXTL);
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

        if(m_oneShot)
        {
            m_timer = m_worker->createTimer();
            m_timer->setOneShot(true);
            m_timer->setFunction(std::bind(&SMatricesReader::readMatrices, this));
            m_timer->start();
        }
        else
        {
            m_timer = m_worker->createTimer();

            ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(1000/m_fps);

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

    if(!m_tsMatrices.empty())
    {
        m_tsMatrices.clear();
    }

    m_tsMatricesCount = 0;
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
        ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL>(s_MATRIXTL);

        TimeStampedMatrices currentMatrices = m_tsMatrices[m_tsMatricesCount];

        ::fwCore::HiResClock::HiResClockType timestamp;

        if(m_createNewTS)
        {
            timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
        }
        else
        {
            timestamp = currentMatrices.timestamp;
        }

        // Push matrix in timeline
        SPTR(::arData::MatrixTL::BufferType) matrixBuf;
        matrixBuf = matrixTL->createBuffer(timestamp);
        matrixTL->pushObject(matrixBuf);

        for(unsigned int i = 0; i < currentMatrices.matrices.size(); ++i)
        {
            float mat[16];
            std::copy(currentMatrices.matrices[i].begin(), currentMatrices.matrices[i].end(), &mat[0]);
            matrixBuf->setElement(mat, i);
        }

        //Notify
        ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = matrixTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sig->asyncEmit(timestamp);

        m_tsMatricesCount++;

    }
}

//------------------------------------------------------------------------------

} // namespace ioTimeline
