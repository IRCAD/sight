/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TestServices.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/SeriesDB.hpp>

fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementationComposite,
                         ::fwData::Composite );
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementationImage,
                         ::fwData::Image );

