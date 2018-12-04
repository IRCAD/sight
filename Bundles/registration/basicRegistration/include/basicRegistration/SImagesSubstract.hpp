/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include "basicRegistration/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace basicRegistration
{

/**
 * @brief Compute the substraction of two images.

 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::basicRegistration::SImagesSubstract">
       <in key="image1" uid="..." />
       <in key="image2" uid="..." />
       <inout key="result" uid="..." />
   </service>
   @endcode
 * @subsection In In
 * - \b image1 [::fwData::Image]: first image.
 * - \b image2 [::fwData::Image]: second image.
 * @subsection InOut InOut
 * - \b result [::fwData::Image]: substract image.
 */

class BASICREGISTRATION_CLASS_API SImagesSubstract : public QObject,
                                                     public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SImagesSubstract)(::fwGui::editor::IEditor) );

    BASICREGISTRATION_API SImagesSubstract() noexcept;

    BASICREGISTRATION_API virtual ~SImagesSubstract() noexcept;

protected:

    BASICREGISTRATION_API virtual void configuring() override;

    /// Overrides
    BASICREGISTRATION_API virtual void starting() override;

    /// Overrides
    BASICREGISTRATION_API virtual void stopping() override;

    /// Overrides
    BASICREGISTRATION_API virtual void updating() override;

    /// Overrides
    BASICREGISTRATION_API virtual void swapping() override;

private Q_SLOTS:

    /// Compute the subtraction between two images.
    void OnCompute();

private:
    QPointer< QPushButton > mpComputeButton;
};

} // namespace basicRegistration
