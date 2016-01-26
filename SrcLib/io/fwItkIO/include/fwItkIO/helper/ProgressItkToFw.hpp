/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
