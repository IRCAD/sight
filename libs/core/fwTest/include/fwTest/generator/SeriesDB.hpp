/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwTest/config.hpp"

#include <fwCore/base.hpp>

namespace fwData
{
class Reconstruction;
}

namespace fwMedData
{
class SeriesDB;
class Series;
class Patient;
class Study;
class Equipment;
class ImageSeries;
class ModelSeries;
class ActivitySeries;
}

namespace fwTest
{
namespace generator
{

/**
 * @brief Contains helper to generate Medical Data (SeriesDB, ImageSeries, ...).
 *
 */
class SeriesDB
{

public:

    /**
     * @brief Creates a SeriesDB.
     * @param nbImgSeries       number of ImageSeries to add in seriesDB
     * @param nbModelSeries     number of ModelSeries to add in seriesDB
     * @param nbActivitySeries  number of ActivitySeries to add in seriesDB
     * @return The created SeriesDB
     */
    FWTEST_API static SPTR(::fwMedData::SeriesDB) createSeriesDB(const unsigned char nbImgSeries,
                                                                 const unsigned char nbModelSeries,
                                                                 const unsigned char nbActivitySeries);

    /// Returns a Patient with dummy informations
    FWTEST_API static SPTR(::fwMedData::Patient) createPatient();

    /// Returns a Study with dummy informations
    FWTEST_API static SPTR(::fwMedData::Study) createStudy();

    /// Returns an Equipment with dummy informations
    FWTEST_API static SPTR(::fwMedData::Equipment) createEquipement();

    /// Generates dummy informations for the series
    FWTEST_API static void generateSeriesInformation(SPTR(::fwMedData::Series) series);

    /// Returns an ImageSeries with a randomized image
    FWTEST_API static SPTR(::fwMedData::ImageSeries) createImageSeries();

    /**
     * @brief Returns a ModelSeries containing random Reconstructions
     * @param nbReconstruction number of Reconstrutions to add in the ModelSeries
     */
    FWTEST_API static SPTR(::fwMedData::ModelSeries) createModelSeries(unsigned char nbReconstruction);

    /// Returns an ActivitySeries with dummy data
    FWTEST_API static SPTR(::fwMedData::ActivitySeries) createActivitySeries();

    /**
     * @brief Generate a reconstruction.
     * @param rec   reconstruction to generate
     * @param index index of the reconstruction in reconstruction list (optional).
     * If index > -1, it will be used to prefix organName (and filename) by the index (ex: '0_Liver...')
     * this can be helpful in tests to ensure taht readers will load reconstruction in the same order than the
     * reconstructionDB.
     */
    FWTEST_API static void generateReconstruction(SPTR(::fwData::Reconstruction) rec, int index = -1);

};

} // namespace generator
} // namespace fwTest
