/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_SELECTOR_HPP_
#define FWWX_SELECTOR_HPP_

#include <vector>

#include <fwGui/ISelector.hpp>

#include "fwWX/config.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

/**
 * @brief   Selector allowing the choice of an element among severals (_selections)
 * @class   Selector.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class FWWX_CLASS_API Selector : public ::fwGui::ISelector
{
public:

    fwCoreClassDefinitionsWithFactoryMacro( (Selector)(::fwGui::ISelector), (()), new Selector );

    /// Default constructor.
    FWWX_API Selector() ;

    FWWX_API virtual ~Selector();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWWX_API virtual void setSelections(std::vector< std::string > _selections);

    /**
     * @brief Sets the selector title.
     */
    FWWX_API virtual void setTitle(std::string _title);

    /**
     * @brief Show the selector and return the selection.
     * @param[in] _parent Parent container for the selector.
     */
    FWWX_API virtual std::string show();

private :

    std::vector< std::string > m_selections;

    std::string m_title;
};


}

#endif /*FWWX_SELECTOR_HPP_*/
