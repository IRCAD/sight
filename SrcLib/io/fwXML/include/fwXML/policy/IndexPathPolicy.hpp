/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_INDEXPATHPOLICY_HPP_
#define _FWXML_POLICY_INDEXPATHPOLICY_HPP_

#include <map>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

#include "fwXML/policy/IPathPolicy.hpp"

namespace fwXML
{
/**
 * @brief This class implements the IPathPolicy.
 * Path : <className>_<IndexNumber>.xml
 * An Index is created for each className.
 */
class FWXML_CLASS_API IndexPathPolicy : public IPathPolicy
 {
public :

    fwCoreClassDefinitionsWithFactoryMacro((IndexPathPolicy)(IPathPolicy), ( () ), new IndexPathPolicy );

    /**
      * @brief  defines a path to save an Object
      * Path : <className>_<IndexNumber>.xml
      * An Index is created for each className.
      * @param obj object to save
      * @return path of the Object
      */
    FWXML_API virtual ::boost::filesystem::path getPath(::fwData::Object::sptr obj) ;

private :

    /// store lastest index available  for "className"
    std::map<std::string, int> m_lastIndexAvailable ;

    /// maintain object addr <-> index
    std::map< ::fwData::Object::wptr, int> m_index ;

 }; //class IndexPathPolicy

}//namespace fwXML

#endif // _FWXML_POLICY_INDEXPATHPOLICY_HPP_

