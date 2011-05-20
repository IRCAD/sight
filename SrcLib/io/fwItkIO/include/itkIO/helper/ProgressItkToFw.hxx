/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <limits>
#include <sstream>

#include <itkLightProcessObject.h>
#include <itkEventObject.h>
#include <itkCommand.h>
#include <itkSmartPointer.h>

#include <fwCore/base.hpp>

namespace itkIO
{

class LocalCommand : public itk::Command
{
public:

    LocalCommand() {};

    typedef LocalCommand Self;
    typedef itk::SmartPointer<LocalCommand>      Pointer;
    itkNewMacro (Self);

    void Execute(itk::Object *caller, const itk::EventObject &event)
    {
        itk::LightProcessObject* po = dynamic_cast<itk::LightProcessObject*>(caller);
        if( !po ) return;
        float percent = po->GetProgress();
        OSLM_TRACE("LocalCommand::Execute" << m_msg << " " << percent );
        m_adviser->notifyProgress( percent , m_msg );
    }

    void Execute(const itk::Object *caller, const itk::EventObject &event)
    {
        itk::LightProcessObject* po = dynamic_cast<itk::LightProcessObject*>( const_cast<itk::Object*>(caller));
        Execute(po,event);
    }

    std::string m_msg;
    SPTR(::fwTools::ProgressAdviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::ProgressItkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser) observer, std::string msg)
: m_observee( observee), m_obsTag(std::numeric_limits<unsigned long>::max()), m_initialized(false)
{
    typename LocalCommand::Pointer itkCallBack;
    itkCallBack = LocalCommand::New();
    itkCallBack->m_msg = msg;
    itkCallBack->m_adviser = observer;
    m_obsTag = m_observee->AddObserver(itk::ProgressEvent(), itkCallBack );
    m_initialized = true;
}

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::~ProgressItkToFw()
{
    if( m_initialized)
    {
        m_observee->RemoveObserver (m_obsTag);
    }
}

}

