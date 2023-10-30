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

#include "filter/dicom/composite/base.hpp"

namespace sight::filter::dicom::composite
{

base::base()
= default;

//-----------------------------------------------------------------------------

base::~base()
= default;

//-----------------------------------------------------------------------------

filter::filter_t base::get_filter_type() const
{
    return filter::composite;
}

//-----------------------------------------------------------------------------

base::dicom_series_container_t base::apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
) const
{
    dicom_series_container_t result;
    result.push_back(_series);
    // For every filter
    for(const sight::filter::dicom::filter::sptr& filter : m_filter_container)
    {
        dicom_series_container_t filtered;
        // For every series
        for(const data::dicom_series::sptr& s : result)
        {
            dicom_series_container_t tempo = filter->apply(s, _logger);
            filtered.reserve(filtered.size() + tempo.size());
            std::copy(tempo.begin(), tempo.end(), std::back_inserter(filtered));
        }

        result = filtered;
    }

    return result;
}

//-----------------------------------------------------------------------------

base::dicom_series_container_t base::forced_apply(
    const data::dicom_series::sptr& _series,
    const core::log::logger::sptr& _logger
) const
{
    dicom_series_container_t result;
    result.push_back(_series);
    // For every filters
    for(const sight::filter::dicom::filter::sptr& filter : m_filter_container)
    {
        dicom_series_container_t filtered;
        // For every series
        for(const data::dicom_series::sptr& s : result)
        {
            try
            {
                dicom_series_container_t tempo = filter->apply(s, _logger);
                filtered.reserve(filtered.size() + tempo.size());
                std::copy(tempo.begin(), tempo.end(), std::back_inserter(filtered));
            }
            catch(sight::filter::dicom::exceptions::filter_failure&)
            {
                SIGHT_WARN("Unable to apply the filter \"" << filter->get_name() << "\".");
                filtered.push_back(s);
            }
        }

        result = filtered;
    }

    return result;
}

//-----------------------------------------------------------------------------

void base::add_child(const sight::filter::dicom::filter::sptr& _filter)
{
    m_filter_container.push_back(_filter);
}

//-----------------------------------------------------------------------------

void base::remove_child(const sight::filter::dicom::filter::sptr& _filter)
{
    auto it = std::find(m_filter_container.begin(), m_filter_container.end(), _filter);
    if(it != m_filter_container.end())
    {
        m_filter_container.erase(it);
    }
}

//-----------------------------------------------------------------------------

base::filter_container_t& base::get_children()
{
    return m_filter_container;
}

} // namespace sight::filter::dicom::composite
