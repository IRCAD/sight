/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/http/helper/Series.hpp"

#include <core/spyLog.hpp>

#include <data/DicomSeries.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

#include <boost/foreach.hpp>

namespace sight::io::http::helper
{

// ----------------------------------------------------------------------------

Series::DicomSeriesContainer Series::toFwMedData(const QJsonObject& seriesJson)
{
    DicomSeriesContainer seriesContainer;

    // Create series
    data::DicomSeries::sptr series = data::DicomSeries::New();

    // ==================================
    // Series
    // ==================================

    series->setSeriesInstanceUID(seriesJson["SeriesInstanceUID"].toString().toStdString());
    series->setModality(seriesJson["Modality"].toString().toStdString());
    series->setSeriesDate(seriesJson["SeriesDate"].toString().toStdString());
    series->setSeriesTime(seriesJson["SeriesTime"].toString().toStdString());
    series->setSeriesDescription(seriesJson["SeriesDescription"].toString().toStdString());

    // ==================================
    // Patient
    // ==================================
    series->setPatientName(seriesJson["PatientName"].toString().toStdString());
    series->setPatientID(seriesJson["PatientID"].toString().toStdString());
    series->setPatientBirthDate(seriesJson["PatientBirthDate"].toString().toStdString());
    series->setPatientSex(seriesJson["PatientSex"].toString().toStdString());

    // ==================================
    // Study
    // ==================================
    series->setStudyInstanceUID(seriesJson["StudyInstanceUID"].toString().toStdString());
    series->setStudyDate(seriesJson["StudyDate"].toString().toStdString());
    series->setStudyTime(seriesJson["StudyTime"].toString().toStdString());
    series->setStudyDescription(seriesJson["StudyDescription"].toString().toStdString());
    series->setPatientAge(seriesJson["PatientAge"].toString().toStdString());

    // ==================================
    // Equipment
    // ==================================
    series->setInstitutionName(seriesJson["InstitutionName"].toString().toStdString());

    // ==================================
    // Number of instances
    // ==================================
    series->setNumberOfInstances(static_cast<std::size_t>(seriesJson["NumberOfSeriesRelatedInstances"].toInt()));

    // Add series to container
    seriesContainer.push_back(series);

    return seriesContainer;
}

// ----------------------------------------------------------------------------

Series::InstanceUIDContainer Series::toSeriesInstanceUIDContainer(DicomSeriesContainer series)
{
    InstanceUIDContainer result;

    for(const data::Series::sptr& s : series)
    {
        result.push_back(s->getSeriesInstanceUID());
    }

    return result;
}

} // namespace sight::io::http::helper
