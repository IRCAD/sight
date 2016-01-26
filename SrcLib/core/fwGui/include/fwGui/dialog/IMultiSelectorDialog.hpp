/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_IMULTISELECTORDIALOG_HPP__
#define __FWGUI_DIALOG_IMULTISELECTORDIALOG_HPP__

#include <map>
#include <vector>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   IMultiSelectorDialog allows the choice of some elements among several (_selections)
 * @class   IMultiSelectorDialog
 *
 * @date    2009-2010.
 */
class FWGUI_CLASS_API IMultiSelectorDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IMultiSelectorDialog)(::fwGui::GuiBaseObject) )

    typedef std::map< std::string, bool > Selections;
    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    FWGUI_API IMultiSelectorDialog();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IMultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUI_API virtual void setSelections(Selections _selections) = 0;

    /**
     * @brief Sets the selector title.
     */
    FWGUI_API virtual void setTitle(std::string _title) = 0;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUI_API virtual Selections show() = 0;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg) = 0;
};

} //namespace dialog
} //namespace fwGui

#endif /*__FWGUI_DIALOG_IMULTISELECTORDIALOG_HPP__*/
