/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#ifndef __FWITKIO_HELPER_PROGRESSITKTOFW_HXX__
#define __FWITKIO_HELPER_PROGRESSITKTOFW_HXX__

#include <fwCore/base.hpp>

#include <itkCommand.h>
#include <itkEventObject.h>
#include <itkLightProcessObject.h>
#include <itkSmartPointer.h>

#include <limits>
#include <sstream>

namespace fwItkIO
{

class LocalCommand : public itk::Command
{
public:

    LocalCommand()
    {
    }

    typedef LocalCommand Self;
    typedef itk::SmartPointer<LocalCommand>      Pointer;
    itkNewMacro(Self);

    //------------------------------------------------------------------------------

    void Execute(itk::Object* caller, const itk::EventObject& event) override
    {
        itk::LightProcessObject* po = dynamic_cast<itk::LightProcessObject*>(caller);
        if( !po )
        {
            return;
        }
        float percent = po->GetProgress();
        OSLM_TRACE("LocalCommand::Execute '" << m_msg << "' " << percent );
        m_adviser->notifyProgress( percent, m_msg );
    }

    //------------------------------------------------------------------------------

    void Execute(const itk::Object* caller, const itk::EventObject& event) override
    {
        itk::LightProcessObject* po = dynamic_cast<itk::LightProcessObject*>( const_cast<itk::Object*>(caller));
        Execute(po, event);
    }

    std::string m_msg;
    SPTR(::fwTools::ProgressAdviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::ProgressItkToFw(OBSERVEE observee, SPTR(::fwTools::ProgressAdviser)observer,
                                            std::string msg) :
    m_observee( observee),
    m_obsTag(std::numeric_limits<unsigned long>::max()),
    m_initialized(false)
{
    typename LocalCommand::Pointer itkCallBack;
    itkCallBack            = LocalCommand::New();
    itkCallBack->m_msg     = msg;
    itkCallBack->m_adviser = observer;
    m_obsTag               = m_observee->AddObserver(itk::ProgressEvent(), itkCallBack );
    m_initialized          = true;
}

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::~ProgressItkToFw()
{
    if( m_initialized)
    {
        m_observee->RemoveObserver(m_obsTag);
    }
}

}

#endif // __FWITKIO_HELPER_PROGRESSITKTOFW_HXX__
