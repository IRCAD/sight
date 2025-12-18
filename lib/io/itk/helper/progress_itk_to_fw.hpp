/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <itkCommand.h>
#include <itkProcessObject.h>

#include <core/progress/observer.hpp>
#include <memory>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

class progressor_base
{
public:

    using sptr = std::shared_ptr<progressor_base>;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
class progress_itk_to_fw : public progressor_base
{
public:

    progress_itk_to_fw(OBSERVEE _observee, SPTR(core::progress::observer)_observer);
    virtual ~progress_itk_to_fw();

private:

    OBSERVEE m_observee;
    // observertag used by itk
    std::uint64_t m_obs_tag;
};

//------------------------------------------------------------------------------

class progressor
{
public:

    using sptr = std::shared_ptr<progressor>;

    template<typename OBS>
    progressor(OBS _filter, SPTR(core::progress::observer)_observer) :
        m_progressor(std::make_shared<progress_itk_to_fw<OBS> >(_filter, _observer))
    {
    }

    progressor_base::sptr m_progressor;
};

} // namespace sight::io::itk

#include "io/itk/helper/progress_itk_to_fw.hxx"
