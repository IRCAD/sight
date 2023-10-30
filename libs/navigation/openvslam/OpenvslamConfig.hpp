/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "navigation/openvslam/config.hpp"

namespace sight::navigation::openvslam
{

/**
 * @brief Handles Openvslam orb parameters.
 */
struct NAVIGATION_OPENVSLAM_CLASS_API orb_params
{
    /// Maximum number of key points.
    unsigned int max_num_key_pts = 2000;
    ///
    float scale_factor = 1.2F;
    /// Number of levels
    unsigned int num_levels = 8;
    ///
    unsigned int ini_fast_thr = 20;
    ///
    unsigned int min_fast_thr = 7;
};

/**
 * @brief Handles Openvslam Initializer parameters (only used for monocular).
 */
struct NAVIGATION_OPENVSLAM_CLASS_API init_params
{
    /// Number of Ransac Iterations.
    unsigned int num_ransac_iterations = 100;
    /// Minimum number of triangulated points.
    unsigned int min_num_triangulated_pts = 50;
    /// Parallax threshold  (in degree).
    float parallax_deg_thr = 1.0;
    /// Reprojection Error threshold.
    float reprojection_err_thr = 4.0;
    /// Number of Iterations for bundle-adjustment.
    unsigned int num_ba_iterations = 20;
    /// Initial scale factor.
    float scaling_factor = 1000.0;
};

/**
 * @brief Handles Openvslam tracking parameters.
 */
struct NAVIGATION_OPENVSLAM_CLASS_API tracking_params
{
    /// Depth threshold
    double true_depth_thr = 40.0;
    /// Depthmap factor (pixel_value / depthmap_factor = true_depth)
    double depthmap_factor = 1.0;
};

} //namespace sight::navigation::openvslam
