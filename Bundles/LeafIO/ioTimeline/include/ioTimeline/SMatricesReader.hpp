/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOTIMELINE_SMATRICESREADER_HPP__
#define __IOTIMELINE_SMATRICESREADER_HPP__

#include "ioTimeline/config.hpp"

#include <arData/MatrixTL.hpp>

#include <fwThread/Timer.hpp>

#include <io/IReader.hpp>

#include <array>

namespace ioTimeline
{

/**
 * @brief This service reads a csv file and extract matrices from it to push it into a matrixTL.
 * This service can be used in two ways, first one is full-automatic by setting the framerate (oneShot off),
 * the second one is one-per-one using readNext and/or readPrevious slots.
 *
 * @note Each line of csv file should be written like:
 * timestamp;matrix1-value1;...;matrix1-value16;...;matrixN-value1;...;matrixN-value16;
 * Each line should contain exactly the same number of matrices.
 *
 * @section Slots Slots
 * - \b startReading(): start reading matrices
 * - \b stopReading(): stop reading matrices
 * - \b pause(): pause reading.
 * - \b readNext() : read next matrices
 * - \b readPrevious() : read previous matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioTimeline::SMatricesReader">
       <inout key="matrixTL" uid="..." />
       <oneShot>false</oneShot>
       <fps>30</fps>
       <createTimestamp>false</createTimestamp>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b matrixTL [::arData::MatrixTL]: timeline in which matrices will be pushed.
 * @subsection Configuration Configuration
 * - \b oneShot (optionnal): if true reader will read csv line per line.
 * to read next/previous line you should call readNext/readPrevious. If false reader will read each line with
 *  constant framerate
 * (default: false).
 * - \b fps (optionnal): frequence at which matrices are pushed in the timeline (default 30),
 *  only used if oneShot is off.
 * - \b createTimestamp(optionnal): create a new timestamp instead of using csv file (default: false).
 *
 */

class IOTIMELINE_CLASS_API SMatricesReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMatricesReader)(::io::IReader) );

    /// Constructor.
    IOTIMELINE_API SMatricesReader() throw();

    /// Destructor. Does nothing
    IOTIMELINE_API virtual ~SMatricesReader() throw();

    /// Display a location dialog allowing to select the video file to save
    IOTIMELINE_API virtual void configureWithIHM();

    /// Return file type (::io::FILE)
    IOTIMELINE_API virtual ::io::IOPathType getIOPathType() const;

    /**
     * @brief The TimeStampedMatrices structure handle a list of matrices and the associated timestamp.
     */
    struct TimeStampedMatrices
    {
        ///timestamp
        ::fwCore::HiResClock::HiResClockType timestamp;
        ///vector of matrices (std::array<float, 16>).
        std::vector< std::array< float, 16 > > matrices;
    };

protected:

    /// Does nothing
    IOTIMELINE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Does nothing
    IOTIMELINE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Does nothing
    IOTIMELINE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing
    IOTIMELINE_API virtual void updating() throw ( ::fwTools::Failed );

private:

    /// SLOT: Read previous TimeStamped matrices
    void readPrevious();
    /// SLOT: Read next TimeStamped matrices
    void readNext();

    /// SLOT: Start reading
    void startReading();

    /// SLOT: Stop reading
    void stopReading();

    /// SLOT: pause reading
    void pause();

    /// Read matrices (this function is set to the worker)
    void readMatrices();

    bool m_isPlaying; ///<flag if the service is playing.

    std::ifstream* m_filestream;///< input filestream

    std::vector< TimeStampedMatrices > m_tsMatrices;///< vector of TimeStampedMatrices read from csv file.

    ::fwThread::Timer::sptr m_timer; ///< Timer to call readMatrices at constant framerate

    ::fwThread::Worker::sptr m_worker; ///< Worker for the readMatrices timer

    size_t m_tsMatricesCount; ///< counter used by matrices reader

    bool m_createNewTS; ///< bool to handle if we create a new timestamp

    unsigned int m_fps; ///< fps used to read matrices

    bool m_oneShot; ///< oneShot mode (read matrices one-by-one)

};

} // ioTimeline

#endif // __IOTIMELINE_SMATRICESREADER_HPP__
