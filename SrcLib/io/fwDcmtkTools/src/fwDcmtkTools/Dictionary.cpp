/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwDcmtkTools/Dictionary.hpp"

#include <fwCore/spyLog.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwDcmtkTools
{

void Dictionary::loadDictionary()
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        ::boost::filesystem::path full_path(::boost::filesystem::initial_path< ::boost::filesystem::path >());
        std::string dicoPath = RC_PREFIX "/fwDcmtkTools_0-1/dicom.dic";
        bool loaded          = dcmDataDict.wrlock().loadDictionary(dicoPath.c_str());
        dcmDataDict.unlock();
        SLM_ERROR_IF("Unable to load DICOM dictionary !", loaded);
        SLM_ASSERT("Unable to load DICOM dictionary !", loaded);
        SLM_TRACE("DICOM dictionary loaded !");
    }
}

} //fwDcmtkTools
