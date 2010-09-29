/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_ALWAYSPLITPOLICY_H_
#define _FWXML_POLICY_ALWAYSPLITPOLICY_H_

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"
#include "fwXML/policy/ISplitPolicy.hpp"

namespace fwXML
{
/**
 * @brief   This class implement de full split policy
 */
class FWXML_CLASS_API AlwaysSplitPolicy : public ISplitPolicy
{
public:
    /**
     * @brief   always return true
     */
    FWXML_API virtual bool split(::boost::shared_ptr< ::fwTools::Object > obj);
};

} //namespace fwXML

#endif // _FWXML_POLICY_ALWAYSPLITPOLICY_H_

