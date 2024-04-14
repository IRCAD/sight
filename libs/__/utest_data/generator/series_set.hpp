/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/utest_data/config.hpp>

#include <core/base.hpp>

namespace sight::data
{

class activity;
class equipment;
class image_series;
class model_series;
class patient;
class reconstruction;
class series_set;
class series;
class study;

}

namespace sight::utest_data
{

namespace generator
{

/**
 * @brief Contains helper to generate Medical Data (series_set, ImageSeries, ...).
 *
 */
class series_set
{
public:

    /**
     * @brief Creates a series_set.
     * @param _nb_img_series       number of ImageSeries to add in series_set
     * @param _nb_model_series     number of ModelSeries to add in series_set
     * @return The created series_set
     */
    SIGHT_UTEST_DATA_API static SPTR(data::series_set) create_series_set(
        const unsigned char _nb_img_series,
        const unsigned char _nb_model_series
    );

    /// Generates dummy informations for the series
    SIGHT_UTEST_DATA_API static void generate_series_information(SPTR(data::series) _series);

    /// Returns an ImageSeries with a randomized image
    SIGHT_UTEST_DATA_API static SPTR(data::image_series) create_image_series();

    /**
     * @brief Returns a ModelSeries containing random Reconstructions
     * @param _nb_reconstruction number of Reconstrutions to add in the ModelSeries
     */
    SIGHT_UTEST_DATA_API static SPTR(data::model_series) create_model_series(unsigned char _nb_reconstruction);

    /// Returns an Activity with dummy data
    SIGHT_UTEST_DATA_API static SPTR(data::activity) create_activity();

    /**
     * @brief Generate a reconstruction.
     * @param _rec   reconstruction to generate
     * @param _index index of the reconstruction in reconstruction list (optional).
     * If index > -1, it will be used to prefix organName (and filename) by the index (ex: '0_Liver...')
     * this can be helpful in tests to ensure that readers will load reconstruction in the same order than the
     * reconstructionDB.
     */
    SIGHT_UTEST_DATA_API static void generate_reconstruction(SPTR(data::reconstruction) _rec, int _index = -1);
};

} // namespace generator

} // namespace sight::utest_data
