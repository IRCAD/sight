/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/tools/progress_adviser.hpp>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

class ProgressorBase
{
public:

    typedef SPTR(ProgressorBase) sptr;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
class ProgressItkToFw : public ProgressorBase
{
public:

    ProgressItkToFw(OBSERVEE observee, SPTR(core::tools::progress_adviser)observer, std::string msg);

    virtual ~ProgressItkToFw();

protected:

    OBSERVEE m_observee;
    // observertag used by itk
    std::uint64_t m_obsTag;
    bool m_initialized {false};
};

//------------------------------------------------------------------------------

class Progressor
{
public:

    typedef SPTR(Progressor) sptr;

    template<typename OBS>
    Progressor(OBS filter, SPTR(core::tools::progress_adviser)observer, std::string message) :
        m_progressor(ProgressorBase::sptr(new ProgressItkToFw<OBS>(filter, observer, message)))
    {
    }

    ProgressorBase::sptr m_progressor;
};

} // namespace sight::io::itk

#include "io/itk/helper/ProgressItkToFw.hxx"
