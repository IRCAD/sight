/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto01/SStringReader.hpp"

#include <fwCore/spyLog.hpp>

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace beginnerTraining
{
namespace tuto01
{

//-----------------------------------------------------------------------------

SStringReader::SStringReader()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

SStringReader::~SStringReader() noexcept
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::starting()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::stopping()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SStringReader::updating()
{
    SLM_TRACE_FUNC();

    if ( this->hasLocationDefined() )
    {
        // Read data.txt
        std::string line;
        std::string data("");
        std::ifstream myfile( this->getFile().string().c_str() );
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
}

//-----------------------------------------------------------------------------

void SStringReader::configureWithIHM()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SStringReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//-----------------------------------------------------------------------------

} // namespace tuto01
} // namespace beginnerTraining
