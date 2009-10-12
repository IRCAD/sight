/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */


#ifndef XMLSUBSTITUTE_HPP_
#define XMLSUBSTITUTE_HPP_


#include <map>
#include <list>
#include <string>

#include <libxml/tree.h>


namespace fwRuntime {
namespace io {


struct Substitute{
	std::string xpath;
	std::string dictEntry;
};


void substitute( xmlNodePtr original, xmlNodePtr substitutionRules, std::map< std::string, std::string > &dictionary);


std::list< Substitute > getSubstitutions( xmlNodePtr substitutionRules );


}
}


#endif /* XMLSUBSTITUTE_HPP_ */
