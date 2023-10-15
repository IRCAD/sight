/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "filter/dicom/config.hpp"
#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/factory/new.hpp"
#include "filter/dicom/registry/detail.hpp"

#include <core/log/logger.hpp>
#include <core/tools/object.hpp>

#include <data/dicom_series.hpp>

namespace sight::filter::dicom
{

/**
 * @brief Base class for Dicom instance filter.
 */
class FILTER_DICOM_CLASS_API filter : public core::tools::object
{
public:

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry()
        {
            sight::filter::dicom::registry::get()->add_factory(T::classname(), &sight::filter::dicom::factory::make<T>);
        }
    };

    SIGHT_DECLARE_CLASS(filter, core::tools::object);

    typedef std::vector<data::dicom_series::sptr> DicomSeriesContainerType;

    /**
     * @brief Filter types
     */
    typedef enum
    {
        MODIFIER  = 0, /*! Modifier */
        SORTER    = 1, /*! Sorter */
        SPLITTER  = 2, /*! Splitter */
        COMPOSITE = 3, /*! Composite */
        CUSTOM    = 4  /*! Custom */
    } FilterType;

    /// Destructor
    FILTER_DICOM_API ~filter() override;

    /**
     * @brief Apply the filter
     * @return Returns one or more Dicom Instance Group
     */
    FILTER_DICOM_API virtual DicomSeriesContainerType apply(
        const data::dicom_series::sptr& series,
        const core::log::logger::sptr& logger
    ) const = 0;

    /// Return the name of the filter
    FILTER_DICOM_API virtual std::string getName() const = 0;

    /// Return the description of the filter
    FILTER_DICOM_API virtual std::string getDescription() const = 0;

    /// Return filter type
    FILTER_DICOM_API virtual FilterType getFilterType() const = 0;

    /// Return true if a configuration is required
    FILTER_DICOM_API virtual bool isConfigurationRequired() const;

    /// Return true if the filter is configurable using GUI
    FILTER_DICOM_API virtual bool isConfigurableWithGUI() const;

    /// Configure the filter using GUI dialog
    FILTER_DICOM_API virtual void configureWithGUI();

protected:

    /// Constructor
    FILTER_DICOM_API filter();
};

} // namespace sight::filter::dicom
