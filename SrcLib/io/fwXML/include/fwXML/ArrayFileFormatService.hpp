/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _FWXML_ARRAYFILEFORMATSERVICE_HPP_
#define _FWXML_ARRAYFILEFORMATSERVICE_HPP_

#include "fwXML/config.hpp"
#include "fwXML/IFileFormatService.hpp"

namespace fwXML
{

/**
 * @brief The purpose of this class is to provide an io service to save data
 */
class FWXML_CLASS_API ArrayFileFormatService : public IFileFormatService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (ArrayFileFormatService)(::fwXML::IFileFormatService) ) ;

    FWXML_API ArrayFileFormatService();

    FWXML_API virtual ~ArrayFileFormatService();

    FWXML_API virtual void load();

    FWXML_API virtual void save();

};

}

#endif /*_FWXML_ARRAYFILEFORMATSERVICE_HPP_*/
