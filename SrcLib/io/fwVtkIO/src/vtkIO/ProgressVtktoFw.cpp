/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "vtkIO/ProgressVtktoFw.hpp"


namespace vtkIO
{


namespace internals
{

class TranslateEventCommand : public ::vtkCommand
{
public :

    TranslateEventCommand( ::fwTools::ProgressAdviser  *adviser, const std::string &message )
    : m_adviser(adviser), m_msg(message)
    {
    }

    virtual void Execute( ::vtkObject * caller, unsigned long eventType, void * data)
    {
        SLM_ASSERT("m_adviser no set", m_adviser);;
        vtkAlgorithm *algo = vtkAlgorithm::SafeDownCast( caller );
        SLM_ASSERT( "algo no set" , algo );
        double percent = algo->GetProgress();
        m_adviser->notifyProgress( percent , m_msg );
    }
private :
    ::fwTools::ProgressAdviser  *m_adviser;
    std::string                  m_msg;

};
} // namespace internal



ProgressVtktoFw::ProgressVtktoFw( vtkAlgorithm *observed, ::fwTools::ProgressAdviser *observer, std::string message )
: m_algo(observed)
{
    SLM_ASSERT( "observed or observer no set", m_algo && observer );
    vtkCommand *m_cmd = new ::vtkIO::internals::TranslateEventCommand( observer , message );
    m_algo->AddObserver(vtkCommand::ProgressEvent,m_cmd);
}


ProgressVtktoFw::~ProgressVtktoFw()
{
    m_algo->RemoveObserver(m_cmd);
}



} // namespace vtkIO
