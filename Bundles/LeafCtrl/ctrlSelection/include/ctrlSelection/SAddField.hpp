/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_SADDFIELD_HPP__
#define __CTRLSELECTION_SADDFIELD_HPP__

#include "ctrlSelection/config.hpp"

#include <fwServices/IController.hpp>

namespace ctrlSelection
{

/**
 * @brief   Add one or several fields to an object.
 *
 * @deprecated Use SManage instead
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ctrlSelection::SAddField">
            <inout group="source">
                <key uid="..." field="name1" />
                <key uid="..." field="name2" />
            </inout>
            <inout key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b source [::fwData::Object]: object that will be put as field of the target object.
 * @subsection In-Out In-Out:
 * - \b target [::fwData::Object]: object receiving the source object as field.
 * @subsection Configuration Configuration:
 * - \b field Name of the field in the field map of the target object.
 */
class CTRLSELECTION_CLASS_API SAddField : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro((SAddField)(::fwServices::IController));

    /// Constructor.
    CTRLSELECTION_API SAddField() noexcept;

    /// Destructor.
    virtual ~SAddField() noexcept;

protected:

    /// Configure the service.
    CTRLSELECTION_API void configuring() override;

    /// Do nothing.
    CTRLSELECTION_API void starting() override;

    /// Do nothing.
    CTRLSELECTION_API void stopping() override;

    /// Add the source object as fields of the target object.
    CTRLSELECTION_API void updating() override;

private:

    /// Contain all fields names
    std::vector<std::string> m_fields;
};

} //namespace ctrlSelection

#endif  // __CTRLSELECTION_SADDFIELD_HPP__
