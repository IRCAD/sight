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

#pragma once

#include "modules/io/matrix/config.hpp"

#include <core/thread/Timer.hpp>

#include <data/MatrixTL.hpp>

#include <io/base/service/IReader.hpp>

#include <array>

namespace sight::module::io::matrix
{

/**
 * @brief This service reads a csv file and extract matrices from it to push it into a matrixTL.
 *
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
 * - \b setStep(int step, std::string key) : set the step value between two matrices when calling readNext/readPrevious
 * slots on oneShot mode (supported key: "step")
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::SMatricesReader">
       <inout key="matrixTL" uid="..." />
       <oneShot>false</oneShot>
       <fps>30</fps>
       <useTimelapse>true</useTimelapse>
       <createTimestamp>false</createTimestamp>
       <windowTitle>Select the matrix timeline to load</windowTitle>
       <step>5</step>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b matrixTL [sight::data::MatrixTL]: timeline in which matrices will be pushed.
 * @subsection Configuration Configuration
 * - \b oneShot (optionnal): if true reader will read csv line per line.
 * to read next/previous line you should call readNext/readPrevious. If false reader will read each line with
 *  constant framerate
 * (default: false).
 * - \b fps (optionnal): frequence at which matrices are pushed in the timeline (default 30),
 *  only used if oneShot is off.
 * - \b createTimestamp(optionnal): create a new timestamp instead of using csv file (default: false).
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IReader
 * - \b useTimelapse: if set to true, ignore the fps value and use the matrix
 *     timestamps to figure out at which speed to read the matrices. (default: false)
 * - \b step (optionnal): value to jump between two matrices when calling readNext/readPrevious slots (default: 1)
 */

class MODULE_IO_MATRIX_CLASS_API SMatricesReader : public sight::io::base::service::IReader
{
public:

    SIGHT_DECLARE_SERVICE(SMatricesReader, sight::io::base::service::IReader);

    /// Constructor.
    MODULE_IO_MATRIX_API SMatricesReader() noexcept;

    /// Destructor. Does nothing
    MODULE_IO_MATRIX_API virtual ~SMatricesReader() noexcept;

    /// Display a location dialog allowing to select the video file to save
    MODULE_IO_MATRIX_API void openLocationDialog() override;

    /// Return file type (io::base::service::FILE)
    MODULE_IO_MATRIX_API sight::io::base::service::IOPathType getIOPathType() const override;

    /**
     * @brief The TimeStampedMatrices structure handle a list of matrices and the associated timestamp.
     */
    struct TimeStampedMatrices
    {
        ///timestamp
        core::HiResClock::HiResClockType timestamp;
        ///vector of matrices (std::array<float, 16>).
        std::vector<std::array<float, 16> > matrices;
    };

protected:

    /// Does nothing
    MODULE_IO_MATRIX_API void configuring() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void starting() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void stopping() override;

    /// Does nothing
    MODULE_IO_MATRIX_API void updating() override;

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

    /// SLOT: Set step used on readPrevious/readNext slots
    void setStep(int _step, std::string _key);

    /// Read matrices (this function is set to the worker)
    void readMatrices();

    bool m_isPlaying; ///<flag if the service is playing.

    std::ifstream* m_filestream; ///< input filestream

    std::vector<TimeStampedMatrices> m_tsMatrices; ///< vector of TimeStampedMatrices read from csv file.

    core::thread::Timer::sptr m_timer; ///< Timer to call readMatrices at constant framerate

    core::thread::Worker::sptr m_worker; ///< Worker for the readMatrices timer

    size_t m_tsMatricesCount; ///< counter used by matrices reader

    bool m_createNewTS; ///< bool to handle if we create a new timestamp

    unsigned int m_fps; ///< fps used to read matrices (represent the matrices-per-second rate)

    bool m_oneShot; ///< oneShot mode (read matrices one-by-one)

    /// If set to true, ignore the fps value and use the interval between timestamps for the timer
    bool m_useTimelapse;

    /// Step between two matrices when calling readNext()/readPrevious() slots
    unsigned long m_step;
    /// Step value updated in setStep() slot used to compute a shift value when calling readPrevious()/readNext() slots
    unsigned long m_stepChanged;
};

} // ioTimeline
