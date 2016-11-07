/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto03/SStringReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/spyLog.hpp>

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

fwServicesRegisterMacro( ::io::IReader, ::beginnerTraining::tuto03::SStringReader, ::fwData::String );

namespace beginnerTraining
{
namespace tuto03
{

//-----------------------------------------------------------------------------

SStringReader::SStringReader()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

SStringReader::~SStringReader() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if ( this->hasLocationDefined() )
    {
        // Read data.txt
        std::string line;
        std::string data ("");
        std::ifstream myfile ( this->getFile().string().c_str() );
        if ( myfile.is_open() )
        {
            while ( myfile.good() )
            {
                getline( myfile, line );
                OSLM_DEBUG("Read line : " << line );
                data += line;
            }
            myfile.close();
        }
        else
        {
            OSLM_ERROR("Unable to open file : " << this->getFile() );
        }
        OSLM_DEBUG("Loaded data : " << data );

        // Set new string value in your associated object
        auto myAssociatedData = this->getInOut< ::fwData::String >("targetString");
        SLM_ASSERT("Data not found", myAssociatedData);

        myAssociatedData->setValue( data );
    }
    else
    {
        SLM_WARN("Be careful, reader failed because no file location is defined." );
    }

    // Then, notifies listeners that the image has been modified
    this->notifyMessage();
}

//-----------------------------------------------------------------------------

void SStringReader::configureWithIHM()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

::io::IOPathType SStringReader::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void SStringReader::notifyMessage()
{
    SLM_TRACE_FUNC();
    ::fwData::String::sptr associatedObj = this->getObject< ::fwData::String >();

    // Notifies to all service listeners
    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = associatedObj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

} // namespace tuto03
} // namespace beginnerTraining


