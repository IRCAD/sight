/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "utest_data/config.hpp"

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
     * @param nbImgSeries       number of ImageSeries to add in series_set
     * @param nbModelSeries     number of ModelSeries to add in series_set
     * @param nbActivity  number of Activity to add in series_set
     * @return The created series_set
     */
    UTEST_DATA_API static SPTR(data::series_set) createSeriesSet(
        const unsigned char _nb_img_series,
        const unsigned char _nb_model_series
    );

    /// Generates dummy informations for the series
    UTEST_DATA_API static void generateSeriesInformation(SPTR(data::series) _series);

    /// Returns an ImageSeries with a randomized image
    UTEST_DATA_API static SPTR(data::image_series) createImageSeries();

    /**
     * @brief Returns a ModelSeries containing random Reconstructions
     * @param nbReconstruction number of Reconstrutions to add in the ModelSeries
     */
    UTEST_DATA_API static SPTR(data::model_series) createModelSeries(unsigned char _nb_reconstruction);

    /// Returns an Activity with dummy data
    UTEST_DATA_API static SPTR(data::activity) createActivity();

    /**
     * @brief Generate a reconstruction.
     * @param rec   reconstruction to generate
     * @param index index of the reconstruction in reconstruction list (optional).
     * If index > -1, it will be used to prefix organName (and filename) by the index (ex: '0_Liver...')
     * this can be helpful in tests to ensure that readers will load reconstruction in the same order than the
     * reconstructionDB.
     */
    UTEST_DATA_API static void generateReconstruction(SPTR(data::reconstruction) _rec, int _index = -1);
};

} // namespace generator

} // namespace sight::utest_data
