/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_FRAME_SDEFAULTFRAME_HPP__
#define __GUI_FRAME_SDEFAULTFRAME_HPP__

#include "gui/config.hpp"

#include <fwGui/IFrameSrv.hpp>

#include <fwTools/Failed.hpp>

namespace gui
{
namespace frame
{

/**
 * @brief Defines the default frame for standard application.
 */
class GUI_CLASS_API SDefaultFrame : public ::fwGui::IFrameSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SDefaultFrame)(::fwGui::IFrameSrv) );

    /// Constructor. Do nothing.
    GUI_API SDefaultFrame() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultFrame() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see ::fwGui::IFrameSrv::initialize()
     */
    GUI_API virtual void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see ::fwGui::IFrameSrv::create()
     */
    GUI_API virtual void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see ::fwGui::IFrameSrv::destroy()
     */
    GUI_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;
    ///@}

};

} // namespace frame
} // namespace gui

#endif /*__GUI_FRAME_SDEFAULTFRAME_HPP__*/
