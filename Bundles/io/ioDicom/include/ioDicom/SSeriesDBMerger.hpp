/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __IODICOM_SSERIESDBMERGER_HPP__
#define __IODICOM_SSERIESDBMERGER_HPP__

#include "ioDicom/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <boost/filesystem/path.hpp>

namespace ioDicom
{

/**
 * @brief   This service is used to merge selected series with a SeriesDB.
 *          All the series in the vector are pushed in the destination SeriesDB.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicom::SSeriesDBMerger">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [::fwData::Vector]: Vector containing the selected series to merge.
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB where to put the series.
 */
class IODICOM_CLASS_API SSeriesDBMerger : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro( (SSeriesDBMerger)( ::fwGui::IActionSrv ) );

    /// Constructor
    IODICOM_API SSeriesDBMerger() noexcept;

    /// Destructor
    IODICOM_API virtual ~SSeriesDBMerger() noexcept;

protected:

    /// Do nothing.
    IODICOM_API virtual void configuring() override;

    /// Override
    IODICOM_API virtual void starting() override;

    /// Override
    IODICOM_API virtual void stopping() override;

    /// Override
    IODICOM_API void updating() override;

    /// Override
    IODICOM_API void info(std::ostream& _sstream ) override;
};

} // namespace ioDicom

#endif // __IODICOM_SSERIESDBMERGER_HPP__
