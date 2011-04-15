/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_PROGESSVTKTOFW_HPP_
#define _VTKIO_PROGESSVTKTOFW_HPP_

#include <vtkCommand.h>
#include <vtkProcessObject.h>

#include <fwTools/ProgressAdviser.hpp>

namespace vtkIO
{

//------------------------------------------------------------------------------

class ProgressorBase
{
    public:
    typedef SPTR(ProgressorBase) sptr;
};

//------------------------------------------------------------------------------

template< typename OBSERVEE >
class ProgressVtkToFw : public ProgressorBase
{
public:
    ProgressVtkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser) observer, std::string msg);

    virtual ~ProgressVtkToFw();

protected :

    OBSERVEE m_observee;
    // observertag used by vtk
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
        typedef ProgressVtkToFw< OBS > ProgressType;
        m_progressor = ProgressorBase::sptr(
                new ProgressType( filter, observer, message )
                );
    }

    ProgressorBase::sptr m_progressor;
};

}


#include "vtkIO/helper/ProgressVtkToFw.hxx"

#endif /* _VTKIO_PROGESSVTKTOFW_HPP_ */
