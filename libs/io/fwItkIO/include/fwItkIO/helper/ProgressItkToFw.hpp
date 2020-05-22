/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#ifndef __FWITKIO_HELPER_PROGRESSITKTOFW_HPP__
#define __FWITKIO_HELPER_PROGRESSITKTOFW_HPP__

#include <itkCommand.h>
#include <itkProcessObject.h>

#include <fwTools/ProgressAdviser.hpp>

namespace fwItkIO
{

//------------------------------------------------------------------------------

class ProgressorBase
{
public:
    typedef SPTR (ProgressorBase) sptr;
};

//------------------------------------------------------------------------------

template< typename OBSERVEE >
class ProgressItkToFw : public ProgressorBase
{
public:
    ProgressItkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser)observer, std::string msg);

    virtual ~ProgressItkToFw();

protected:

    OBSERVEE m_observee;
    // observertag used by itk
    unsigned long m_obsTag;
    bool m_initialized;
};

//------------------------------------------------------------------------------

class Progressor
{
public:
    typedef SPTR (Progressor) sptr;

    template<typename OBS >
    Progressor(OBS filter, SPTR(::fwTools::ProgressAdviser)observer, std::string message)
    {
        typedef ProgressItkToFw< OBS > ProgressType;
        m_progressor = ProgressorBase::sptr(
            new ProgressType( filter, observer, message )
            );
    }

    ProgressorBase::sptr m_progressor;
};

}

#include "fwItkIO/helper/ProgressItkToFw.hxx"

#endif /* __FWITKIO_HELPER_PROGRESSITKTOFW_HPP__ */
