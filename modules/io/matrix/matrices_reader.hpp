/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <core/thread/timer.hpp>

#include <data/matrix_tl.hpp>

#include <io/__/service/reader.hpp>

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
 * - \b start_reading(): start reading matrices
 * - \b stop_reading(): stop reading matrices
 * - \b pause(): pause reading.
 * - \b readNext() : read next matrices
 * - \b readPrevious() : read previous matrices
 * - \b set_step(int step, std::string key) : set the step value between two matrices when calling readNext/readPrevious
 * slots on oneShot mode (supported key: "step")
 * - \b toggle_loop_mode() : changes the loop mode. If active, the reader loops over the file,
 * if false, it reads the file once only
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::matrix::matrices_reader">
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
 * - \b matrixTL [sight::data::matrix_tl]: timeline in which matrices will be pushed.
 * @subsection Configuration Configuration
 * - \b oneShot (optional): if true reader will read csv line per line.
 * to read next/previous line you should call readNext/readPrevious. If false reader will read each line with
 *  constant framerate
 * (default: false).
 * - \b fps (optional): frequency at which matrices are pushed in the timeline (default 30),
 *  only used if oneShot is off.
 * - \b createTimestamp(optional): create a new timestamp instead of using csv file (default: false).
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::reader
 * - \b useTimelapse: if set to true, ignore the fps value and use the matrix
 *     timestamps to figure out at which speed to read the matrices. (default: false)
 * - \b step (optional): value to jump between two matrices when calling readNext/readPrevious slots (default: 1)
 * - \b loop (optional): specifies if the reader loops over the file or not (default: false)
 */

class matrices_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(matrices_reader, sight::io::service::reader);

    /// Constructor.
    matrices_reader() noexcept;

    /// Destructor. Does nothing
    ~matrices_reader() noexcept override;

    /// Display a location dialog allowing to select the video file to save
    void open_location_dialog() override;

    /// Return file type (io::service::FILE)
    sight::io::service::path_type_t get_path_type() const override;

    /**
     * @brief The TimeStampedMatrices structure handle a list of matrices and the associated timestamp.
     */
    struct time_stamped_matrices
    {
        ///timestamp
        core::clock::type timestamp {};
        ///vector of matrices (std::array<float, 16>).
        std::vector<std::array<float, 16> > matrices;
    };

protected:

    /// Does nothing
    void configuring() override;

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Does nothing
    void updating() override;

private:

    /// SLOT: Read previous TimeStamped matrices
    void read_previous();
    /// SLOT: Read next TimeStamped matrices
    void read_next();

    /// SLOT: Start reading
    void start_reading();

    /// SLOT: Stop reading
    void stop_reading();

    /// SLOT: pause reading
    void pause();

    /// SLOT: Set step used on readPrevious/readNext slots
    void set_step(int _step, std::string _key);

    /// SLOT: toggle the loop mode
    void toggle_loop_mode();

    /// Read matrices (this function is set to the worker)
    void read_matrices();

    bool m_is_playing {false}; ///<flag if the service is playing.

    std::ifstream* m_filestream {nullptr}; ///< input filestream

    std::vector<time_stamped_matrices> m_ts_matrices; ///< vector of TimeStampedMatrices read from csv file.

    core::thread::timer::sptr m_timer; ///< Timer to call readMatrices at constant framerate

    core::thread::worker::sptr m_worker; ///< Worker for the readMatrices timer

    std::size_t m_ts_matrices_count {0}; ///< counter used by matrices reader

    bool m_create_new_ts {false}; ///< bool to handle if we create a new timestamp

    unsigned int m_fps {30}; ///< fps used to read matrices (represent the matrices-per-second rate)

    bool m_one_shot {false}; ///< oneShot mode (read matrices one-by-one)

    /// If set to true, ignore the fps value and use the interval between timestamps for the timer
    bool m_use_timelapse {false};

    /// If true: the grabber is paused.
    bool m_is_paused {false};

    /// If set to true, loop over the file
    bool m_loop_matrix {false};

    /// Step between two matrices when calling readNext()/readPrevious() slots
    std::uint64_t m_step {1};
    /// Step value updated in set_step() slot used to compute a shift value when calling readPrevious()/readNext() slots
    std::uint64_t m_step_changed {1};

    static constexpr std::string_view MATRIXTL = "matrixTL";
    sight::data::ptr<sight::data::matrix_tl, sight::data::access::inout> m_matrix_tl {this, MATRIXTL};
};

} // namespace sight::module::io::matrix
