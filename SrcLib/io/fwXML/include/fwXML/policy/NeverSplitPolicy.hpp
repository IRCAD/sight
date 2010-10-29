/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_POLICY_NEVERSPLITPOLICY_H_
#define _FWXML_POLICY_NEVERSPLITPOLICY_H_

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

#include "fwXML/policy/ISplitPolicy.hpp"

namespace fwXML
{

class FWXML_CLASS_API NeverSplitPolicy : public ISplitPolicy
{
public:
    /**
     * @brief   true if the object should be spliten, false otherwise
     */
    FWXML_API virtual bool split(::boost::shared_ptr< ::fwTools::Object > obj);

    /**
     * @brief       Virtual destructor
     */
    FWXML_API virtual ~NeverSplitPolicy();

};

} // namespace fwXML

#endif // _FWXML_POLICY_NEVERSPLITPOLICY_H_


