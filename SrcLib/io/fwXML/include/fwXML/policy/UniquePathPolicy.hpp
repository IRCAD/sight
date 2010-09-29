/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_UNIQUENAMEPATHPOLICY_HPP_
#define _FWXML_POLICY_UNIQUENAMEPATHPOLICY_HPP_

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

#include "fwXML/policy/IPathPolicy.hpp"

namespace fwXML
{
/**
 * @brief This class implements the IPathPolicy. by providing always the same path ( setted via ctor or setUniquePath() )
 *
 * @warning the path produced is always the same USE ONLY WE NeverSplitPolicy !!!! else our multi-file
 * will be overwritten in the same name file
 *
 **/
class FWXML_CLASS_API UniquePathPolicy : public IPathPolicy
 {

public :

    /**
     * @brief Constructor
     **/
    FWXML_API UniquePathPolicy();
     
     /**
      * @brief extra constructor which set the unique path
      */
    FWXML_API UniquePathPolicy(const std::string &filename);


     /**
     * @brief Destructor
     **/
    FWXML_API ~UniquePathPolicy();
     
     /**
      * @brief set the unique filename 
      * @note do not not forget the .xml
      * @warning the path produced is always the same USE ONLY WE NeverSplitPolicy !!!!
     **/
     FWXML_API void setUniquePath(const std::string &filename);


     /**
      * @brief  defines a path to save an Object
     *
     * Path : always
     *
     * @param obj object to save
     *
     * @return path of the Object
     **/
     FWXML_API virtual ::boost::filesystem::path getPath(::boost::shared_ptr < ::fwTools::Object > obj ) ;
     
protected :
    
    ::boost::filesystem::path m_uniquePath;
    
     

 }; //class UniquePathPolicy


}//namespace fwXML


#endif // _FWXML_POLICY_UNIQUENAMEPATHPOLICY_HPP_

