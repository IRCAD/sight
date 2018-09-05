/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

namespace fwRenderOgre
{

namespace compositor
{

/// OIT (Ordrer Independent Transparency) techniques supported
enum transparencyTechnique
{
    DEFAULT,
    DEPTHPEELING,
    DUALDEPTHPEELING,
    WEIGHTEDBLENDEDOIT,
    HYBRIDTRANSPARENCY,
    CELSHADING_DEPTHPEELING,
};

} // namespace compositor

} // namespace fwRenderOgre
