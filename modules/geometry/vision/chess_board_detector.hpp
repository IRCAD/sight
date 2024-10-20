/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <data/calibration_info.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <service/controller.hpp>

namespace sight::module::geometry::vision
{

/**
 * @brief Detects chessboards in images and pushes detected points to CalibrationInfo objects.
 *
 * Every update triggers detection on the current input images. The 'recordPoints' slot must be called to store
 * the chessboard positions in the CalibrationInfo structure after a successful detection.
 *
 * @section Signals Signals
 * - \b chessboard_detected(bool): Emitted after trying to detect a chessboard. Sends whether it was detected or not.
 * - \b chessboardFound(): Emitted if a chessboard pattern was recognized in the image.
 *
 * @section Slots Slots
 * - \b record_points(): Request to store the current image in the calibration data, if the chessboard is detected.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::vision::chess_board_detector" >
            <in group="image">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout group="calInfo">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <inout group="detection">
                <key uid="..." />
                <key uid="..." />
            </inout>
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" scale="CHESSBOARD_SCALE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b image [sight::data::image]: images on which we run the detection.
 * @subsection In-Out In-Out:
 * - \b calInfo [sight::data::calibration_info]: calibration objects storing the detected images.
 * - \b detection [sight::data::point_list] (optional): detected chessboard points in image coordinates.
 * @subsection Configuration Configuration:
 * - \b board : preference keys to retrieve the number of squares of the board in width and height as well
 *              as the scaling factor to be applied to the input image.
 */
class chess_board_detector final : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(chess_board_detector, sight::service::controller);

    /// Signal type sent after trying to detect a chessboard in an image. Sends whether detection was successful.
    using chessboard_detected_signal_t = core::com::signal<void (bool)>;

    /// Signal type sent after a successful detection.
    using chessboard_found_signal_t = core::com::signal<void ()>;

    /// Constructor
    chess_board_detector() noexcept;

    /// Destructor
    ~chess_board_detector() noexcept final;

protected:

    /// Configures the service.
    void configuring(const config_t& _config) final;

    /// Initializes the chessboard size from the preferences.
    void starting() final;

    /// Tries to detect chessboards in all input images.
    void updating() final;

    /// Clears the detected points.
    void stopping() final;

    /// Returns proposals to update the service when the input image is modified.
    connections_t auto_connections() const final;

private:

    /// SLOT: stores the last detected chessboard in the CalibrationInfo structure.
    void record_points();

    /// Runs the detection for the given input index.
    void do_detection(std::size_t _image_index);

    /// Signal emitted after detection.
    chessboard_detected_signal_t::sptr m_sig_chessboard_detected;

    /// Signal emitted if a chessboard can be seen in the image.
    chessboard_found_signal_t::sptr m_sig_chessboard_found;

    /// Last detected chessboard points in each image. Null if detection failed.
    std::vector<data::point_list::sptr> m_point_lists;

    /// Last images on which a chessboard was detected. Null if detection failed.
    std::vector<data::image::sptr> m_images;

    static constexpr std::string_view IMAGE_INPUT     = "image";
    static constexpr std::string_view CALINFO_INOUT   = "calInfo";
    static constexpr std::string_view DETECTION_INOUT = "detection";

    data::ptr_vector<data::image, data::access::in> m_image {this, IMAGE_INPUT};
    data::ptr_vector<data::calibration_info, data::access::inout> m_cal_info {this, CALINFO_INOUT};
    data::ptr_vector<data::point_list, data::access::inout> m_detection {this, DETECTION_INOUT};

    /// Width of the chessboard we're looking for.
    sight::data::property<sight::data::integer> m_width {this, "board_width", 11};

    /// Height of the chessboard we're looking for.
    sight::data::property<sight::data::integer> m_height {this, "board_height", 8};

    /// Scale applied to the images before running the detection algorithm.
    sight::data::property<sight::data::real> m_scale {this, "board_scale", 1.};
};

} //namespace sight::module::geometry::vision
