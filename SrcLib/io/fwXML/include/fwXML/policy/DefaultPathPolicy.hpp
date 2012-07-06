/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_DEFAULTPATHPOLICY_HPP_
#define _FWXML_POLICY_DEFAULTPATHPOLICY_HPP_

#include "fwXML/policy/IPathPolicy.hpp"
#include "fwXML/config.hpp"

#include <fwData/Object.hpp>

namespace fwXML
{
/**
 * @brief This class implements the IPathPolicy.
 *
 * Path : <className>_<Ptr>.xml
 *
 **/
class FWXML_CLASS_API DefaultPathPolicy : public IPathPolicy
{
public :

    fwCoreClassDefinitionsWithFactoryMacro((DefaultPathPolicy)(IPathPolicy), ( () ), new DefaultPathPolicy );

     /**
      * @brief  defines a path to save an Object
     *
     * Path : <className>_<UUID>.xml
     *
     * @param obj object to save
     *
     * @return path of the Object
     **/
     FWXML_API virtual  ::boost::filesystem::path getPath( ::fwData::Object::sptr obj ) ;

 }; //class DefaultPathPolicy

}//namespace fwXML

#endif // _FWXML_POLICY_DEFAULTPATHPOLICY_HPP_

