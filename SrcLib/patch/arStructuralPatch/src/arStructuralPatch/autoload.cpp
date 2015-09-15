/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#include <fwAtomsPatch/StructuralPatchDB.hpp>



namespace arStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {

    }

    static runner r;
};
runner runner::r;

} // namespace arStructuralPatch
