/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef OBJECTXMLPARSER_HPP_
#define OBJECTXMLPARSER_HPP_

#include <fwTools/Object.hpp>
#include "fwServices/IXMLParser.hpp"

namespace fwServices
{

/**
 * @class ObjectXMLParser
 * @brief Specific ::fwTools::Object parser. Does nothing as no IService method is specialized: IXMLParser base class methods are therefore considered.
 *
 * @note This implementation is only written to avoid the mixing both the service type IXMLParser and the ::fwTools::Object implementation (for homogeneity).
 * @author IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ObjectXMLParser : public ::fwServices::IXMLParser
{
	public :
    fwCoreServiceClassDefinitionsMacro ( (ObjectXMLParser)(::fwServices::IXMLParser::Baseclass) ) ;

		FWSERVICES_API ObjectXMLParser( ) ;
		FWSERVICES_API virtual ~ObjectXMLParser() ;

};




}

#endif /* IXMLPARSER_HPP_ */
