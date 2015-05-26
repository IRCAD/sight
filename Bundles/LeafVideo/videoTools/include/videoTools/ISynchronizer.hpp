/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_ISYNCHRONIZER_HPP__
#define __VIDEOTOOLS_ISYNCHRONIZER_HPP__

#include "videoTools/config.hpp"

#include <fwServices/IService.hpp>

namespace videoTools
{
/**
 * @brief   ISynchronizer service synchronize objects from timelines
 * @class   ISynchronizer
 */
class VIDEOTOOLS_CLASS_API ISynchronizer : public fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro((ISynchronizer)(fwServices::IService));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API ISynchronizer() throw ();

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~ISynchronizer() throw ();

};

} //namespace videoTools
#endif  // __VIDEOTOOLS_ISYNCHRONIZER_HPP__
