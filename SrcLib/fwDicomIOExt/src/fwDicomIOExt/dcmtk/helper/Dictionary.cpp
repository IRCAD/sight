/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/spyLog.hpp>

#include "fwDicomIOExt/dcmtk/helper/Dictionary.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

void Dictionary::loadDictionary()
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        ::boost::filesystem::path full_path(::boost::filesystem::initial_path< ::boost::filesystem::path >());
        std::string dicoPath = full_path.string() + "/share/" + PRJ_NAME + "_" + FWDICOMIOEXT_VER + "/dicom.dic";
        bool loaded = dcmDataDict.wrlock().loadDictionary(dicoPath.c_str());
        dcmDataDict.unlock();
        SLM_ASSERT("Unable to load DICOM dictionary !", loaded);
        SLM_TRACE("DICOM dictionary loaded !");
    }
}

} //dcmtk
} //helper
} //fwDicomIOExt
