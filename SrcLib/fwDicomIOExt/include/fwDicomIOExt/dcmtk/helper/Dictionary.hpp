/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_HELPER_DICTIONARY_HPP__
#define __FWDICOMIOEXT_DCMTK_HELPER_DICTIONARY_HPP__

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{
namespace helper
{

/**
 * @class Dictionary
 * @brief Dictionary Helper
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API Dictionary
{
public:
    /// Load the DICOM dictionary
    FWDICOMIOEXT_API static void loadDictionary();

};

} //dcmtk
} //helper
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_DCMTK_HELPER_DICTIONARY_HPP__ */
