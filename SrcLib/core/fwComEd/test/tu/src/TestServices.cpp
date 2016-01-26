/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "TestServices.hpp"

fwServicesRegisterMacro( ::fwComEd::ut::TestService, ::fwComEd::ut::TestServiceImplementationComposite,
                         ::fwData::Composite );
fwServicesRegisterMacro( ::fwComEd::ut::TestService, ::fwComEd::ut::TestServiceImplementationImage, ::fwData::Image );

