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

#include "utestData/config.hpp"

#include <core/base.hpp>

namespace sight::data
{

class Reconstruction;

}

namespace sight::data
{

class SeriesSet;
class Series;
class Patient;
class Study;
class Equipment;
class ImageSeries;
class ModelSeries;
class Activity;

}

namespace sight::utestData
{

namespace generator
{

/**
 * @brief Contains helper to generate Medical Data (SeriesSet, ImageSeries, ...).
 *
 */
class SeriesSet
{
public:

    /**
     * @brief Creates a SeriesSet.
     * @param nbImgSeries       number of ImageSeries to add in SeriesSet
     * @param nbModelSeries     number of ModelSeries to add in SeriesSet
     * @param nbActivity  number of Activity to add in SeriesSet
     * @return The created SeriesSet
     */
    UTESTDATA_API static SPTR(data::SeriesSet) createSeriesSet(
        const unsigned char nbImgSeries,
        const unsigned char nbModelSeries
    );

    /// Generates dummy informations for the series
    UTESTDATA_API static void generateSeriesInformation(SPTR(data::Series) series);

    /// Returns an ImageSeries with a randomized image
    UTESTDATA_API static SPTR(data::ImageSeries) createImageSeries();

    /**
     * @brief Returns a ModelSeries containing random Reconstructions
     * @param nbReconstruction number of Reconstrutions to add in the ModelSeries
     */
    UTESTDATA_API static SPTR(data::ModelSeries) createModelSeries(unsigned char nbReconstruction);

    /// Returns an Activity with dummy data
    UTESTDATA_API static SPTR(data::Activity) createActivity();

    /**
     * @brief Generate a reconstruction.
     * @param rec   reconstruction to generate
     * @param index index of the reconstruction in reconstruction list (optional).
     * If index > -1, it will be used to prefix organName (and filename) by the index (ex: '0_Liver...')
     * this can be helpful in tests to ensure that readers will load reconstruction in the same order than the
     * reconstructionDB.
     */
    UTESTDATA_API static void generateReconstruction(SPTR(data::Reconstruction) rec, int index = -1);
};

} // namespace generator

} // namespace sight::utestData
