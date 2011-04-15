/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <limits>
#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkEvent.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>

#include <fwCore/base.hpp>

namespace vtkIO
{

class LocalCommand : public vtkCommand
{
public:

    LocalCommand() {};

    static LocalCommand* New()
    {
        return new LocalCommand();
    }

    typedef LocalCommand Self;
    typedef vtkSmartPointer<Self>      Pointer;

    void Execute(vtkObject *caller, unsigned long eventId, void *callData)
    {
        vtkAlgorithm* algo = vtkAlgorithm::SafeDownCast(caller);
        if( !algo ) return;
        double percent = algo->GetProgress();
        SLM_ASSERT("m_adviser no set", m_adviser);
        m_adviser->notifyProgress( percent , m_msg );
    }

    std::string m_msg;
    SPTR(::fwTools::ProgressAdviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressVtkToFw<OBSERVEE >::ProgressVtkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser) observer, std::string msg)
: m_observee( observee), m_obsTag(std::numeric_limits<unsigned long>::max()), m_initialized(false)
{
    typename LocalCommand::Pointer vtkCallBack;
    vtkCallBack = LocalCommand::Pointer::New();
    vtkCallBack->m_msg = msg;
    vtkCallBack->m_adviser = observer;
    m_obsTag = m_observee->AddObserver(vtkCommand::ProgressEvent, vtkCallBack );
    m_initialized = true;
}

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressVtkToFw<OBSERVEE >::~ProgressVtkToFw()
{
    if( m_initialized)
    {
        m_observee->RemoveObserver (m_obsTag);
    }
}

}

