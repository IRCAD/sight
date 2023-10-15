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

#include "data/model_series.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <data/exception.hpp>
#include <data/image.hpp>
#include <data/reconstruction.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::model_series)

namespace sight::data
{

const core::com::signals::key_t model_series::RECONSTRUCTIONS_ADDED_SIG   = "reconstructionsAdded";
const core::com::signals::key_t model_series::RECONSTRUCTIONS_REMOVED_SIG = "reconstructionsRemoved";

model_series::model_series() :
    has_fiducials(m_signals),
    m_sigReconstructionsAdded(std::make_shared<ReconstructionsAddedSignalType>()),
    m_sigReconstructionsRemoved(std::make_shared<ReconstructionsRemovedSignalType>())
{
    m_signals(RECONSTRUCTIONS_ADDED_SIG, m_sigReconstructionsAdded)
        (RECONSTRUCTIONS_REMOVED_SIG, m_sigReconstructionsRemoved);
}

//------------------------------------------------------------------------------

void model_series::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const model_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_reconstructionDB = other->m_reconstructionDB;
    m_dicomReference   = other->m_dicomReference;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void model_series::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const model_series>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_reconstructionDB.clear();
    for(const data::reconstruction::sptr& rec : other->m_reconstructionDB)
    {
        m_reconstructionDB.push_back(data::object::copy(rec, cache));
    }

    m_dicomReference = data::object::copy(other->m_dicomReference);

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool model_series::operator==(const model_series& other) const noexcept
{
    if(!core::tools::is_equal(m_dicomReference, other.m_dicomReference)
       || !core::tools::is_equal(m_reconstructionDB, other.m_reconstructionDB))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool model_series::operator!=(const model_series& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

fiducials_series::csptr model_series::getFiducials() const
{
    return m_fiducialsSeries;
}

//------------------------------------------------------------------------------

fiducials_series::sptr model_series::getFiducials()
{
    return m_fiducialsSeries;
}

//------------------------------------------------------------------------------

bool model_series::hasFiducials() const
{
    return !m_fiducialsSeries->getFiducialSets().empty();
}

} // namespace sight::data
