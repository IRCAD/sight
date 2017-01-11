/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKTOOLS_DICTIONARY_HPP__
#define __FWDCMTKTOOLS_DICTIONARY_HPP__

#include "fwDcmtkTools/config.hpp"

#include <fwTools/Type.hpp>

#include <stdint.h>

namespace fwDcmtkTools
{

/**
 * @brief DICOM Dictionary Helper
 */
class FWDCMTKTOOLS_CLASS_API Dictionary
{
public:
    /// Load the DICOM dictionary
    FWDCMTKTOOLS_API static void loadDictionary();

};

} //fwDcmtkTools


#endif // __FWDCMTKTOOLS_DICTIONARY_HPP__
