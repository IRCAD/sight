Shortcuts
=========

.. index:: Shortcut, Shortcuts


Global shortcuts
----------------

========== ================================
 Shortcut   Action
========== ================================
 Suppr      Erase selected image
 F1         3D Model view
 F2         2D Medical Image view
 F3         3D Volume rendering view
 F9         Manage organs colors
 F12        HShow/Hide patient information
========== ================================


Global 3D manipulation (in 3D and Volume Rendering view only)
-------------------------------------------------------------

=== ===============
 1    axial view
 2   frontal view
 3   sagittal view
=== ===============




Global 3D-2D manipulation
-------------------------

MD = Mouse drag which mean move the mouse by letting the selected button pressed.

.. list-table::

   * - shift + left mouse button + MD

       shift + middle mouse button + MD

       ctrl + middle mouse button + MD
     - Translation
   * - mouse wheel (up or down)

       shift + Right mouse button + MD

       ctrl + Right mouse button + MD
     - Zoom
   * - R
     - Reset camera





Specific manipulation for medical image (2D or 3D)
--------------------------------------------------

.. list-table::

   * - left/right key

       shift + mouse wheel
     - modify current slice  (after any click in selected slice)
   * - T/Y
     - modify axial slice
   * - G/H
     - modify frontal slice
   * - B/N
     - modify sagittal slice
   * - shift + R
     - reset window/level
   * - left mouse button
     - show pixel values
   * - middle mouse button
     - synchronize all views
   * - right mouse button
     - modify level and window
   * - ctrl + L
     - add a landmark on the last selected voxel
   * - ctrl + F
     - synchronize all views on selected landmark
   * - ctrl + V
     - hide/show landmarks
   * - right mouse click
     - remove distance/landmark



Specific manipulation of the oblique plane
------------------------------------------

======================================================= ========================================================
 middle mouse button                                     in center of plane : plane translation from his normal

                                                         in corner : rotation in the plane

                                                         in border : rotation of the plane around his normal

 ctrl + middle mouse button                              in center of plane : plane translation from his normal

                                                         in corner : translation of the edge around the corner

                                                         in border : translation of the edge

 shift + middle mouse button                             reslice plane (increase/decrease plane size)
======================================================= ========================================================


Specific manipulation of Volume rendering clipping box
------------------------------------------------------


========== ===================================
 O          show/hide clipping box
 Ctrl + O   reset clipping box
 T/Y        clipping box axial translation
 G/H        clipping box frontal translation
 B/N        clipping box sagittal translation
 P          show/hide clipping plane
 Ctrl + P   reset clipping plane
========== ===================================


Specific manipulation of Transfer function editor
-------------------------------------------------

========================= ==================
 left mouse button         select points
 Ctrl + left mouse click   new point
 double left mouse click   edit point color
 right mouse click         remove point
========================= ==================

