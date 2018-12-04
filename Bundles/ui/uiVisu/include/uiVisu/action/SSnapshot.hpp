/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __UIVISU_ACTION_SSNAPSHOT_HPP__
#define __UIVISU_ACTION_SSNAPSHOT_HPP__

#include "uiVisu/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwTools/Failed.hpp>

#include <map>
#include <string>

namespace uiVisu
{
namespace action
{

/**
 * @brief   This action allows to snapshot a generic scene
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class UIVISU_CLASS_API SSnapshot : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSnapshot)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    UIVISU_API SSnapshot() noexcept;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~SSnapshot() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Starts action
    virtual void starting() override;

    /// Stops action
    virtual void stopping() override;

    /// Show a file dialog and notify the scene which must be printed.
    void updating() override;

    /// Does nothing
    void swapping() override;

    /**
     * @brief Configure action.
     * @code{.xml}
       <service type="::fwGui::IActionSrv" impl="::uiData::action::SSnapshot" autoConnect="no" />
       @endcode
     */
    void configuring() override;

    virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Signals
     * @{
     */
    /// Type of signal to snap shot
    typedef ::fwCom::Signal< void (std::string) > SnappedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    std::string requestFileName();

};

} // action
} // uiVisu

#endif // __UIVISU_ACTION_SSNAPSHOT_HPP__

