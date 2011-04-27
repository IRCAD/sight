/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ITKIO_PROGESSITKTOFW_HPP_
#define _ITKIO_PROGESSITKTOFW_HPP_

#include <itkCommand.h>
#include <itkProcessObject.h>

#include <fwTools/ProgressAdviser.hpp>

namespace itkIO
{

//------------------------------------------------------------------------------

class ProgressorBase
{
public:
    typedef SPTR(ProgressorBase) sptr;
};

//------------------------------------------------------------------------------

template< typename OBSERVEE >
class ProgressItkToFw : public ProgressorBase
{
public:
    ProgressItkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser) observer, std::string msg);

    virtual ~ProgressItkToFw();

protected :

    OBSERVEE m_observee;
    // observertag used by itk
    unsigned long m_obsTag;
    bool m_initialized;
};

//------------------------------------------------------------------------------

class Progressor
{
    public:
    typedef SPTR(Progressor) sptr;

    template<typename OBS >
    Progressor(OBS filter, SPTR(::fwTools::ProgressAdviser) observer, std::string message)
    {
        typedef ProgressItkToFw< OBS > ProgressType;
        m_progressor = ProgressorBase::sptr(
                new ProgressType( filter, observer, message )
                );
    }

    ProgressorBase::sptr m_progressor;
};

}


#include "itkIO/helper/ProgressItkToFw.hxx"

#endif /* _ITKIO_PROGESSITKTOFW_HPP_ */
