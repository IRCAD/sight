/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/filter/dicom/config.hpp>

#include "filter/dicom/exceptions/filter_failure.hpp"
#include "filter/dicom/factory/new.hpp"
#include "filter/dicom/registry/detail.hpp"

#include <core/log/logger.hpp>
#include <core/object.hpp>

#include <data/dicom_series.hpp>

namespace sight::filter::dicom
{

/**
 * @brief Base class for Dicom instance filter.
 */
class SIGHT_FILTER_DICOM_CLASS_API filter : public core::object
{
public:

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T factory product type
     */
    template<typename T>
    class registry
    {
    public:

        registry()
        {
            sight::filter::dicom::registry::get()->add_factory(T::classname(), &sight::filter::dicom::factory::make<T>);
        }
    };

    SIGHT_DECLARE_CLASS(filter, core::object);

    using dicom_series_container_t = std::vector<data::dicom_series::sptr>;

    /**
     * @brief filter types
     */
    enum filter_t
    {
        modifier  = 0, /*! Modifier */
        sorter    = 1, /*! Sorter */
        splitter  = 2, /*! Splitter */
        composite = 3, /*! Composite */
        custom    = 4  /*! Custom */
    };

    /// Destructor
    SIGHT_FILTER_DICOM_API ~filter() override;

    /**
     * @brief Apply the filter
     * @return Returns one or more Dicom Instance Group
     */
    SIGHT_FILTER_DICOM_API virtual dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const = 0;

    /// Return the name of the filter
    SIGHT_FILTER_DICOM_API virtual std::string get_name() const = 0;

    /// Return the description of the filter
    SIGHT_FILTER_DICOM_API virtual std::string get_description() const = 0;

    /// Return filter type
    SIGHT_FILTER_DICOM_API virtual filter_t get_filter_type() const = 0;

    /// Return true if a configuration is required
    SIGHT_FILTER_DICOM_API virtual bool is_configuration_required() const;

    /// Return true if the filter is configurable using GUI
    SIGHT_FILTER_DICOM_API virtual bool is_configurable_with_gui() const;

    /// Configure the filter using GUI dialog
    SIGHT_FILTER_DICOM_API virtual void configure_with_gui();

protected:

    /// Constructor
    SIGHT_FILTER_DICOM_API filter();
};

} // namespace sight::filter::dicom
