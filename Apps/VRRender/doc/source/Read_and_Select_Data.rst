Read and select data 
====================

.. index:: Read Data, Select Data



It is possible to load recorded patient by using the ``Import`` functions in the ``File`` menu. Three formats are available for reading : DICOM, MFO (VR-Anat IRCAD Format) and FXZ (VR-Planning IRCAD Format). The Dicom format is available through the first button of the tool bar, the two IRCAD formats being available from the second button :

.. image:: _static/openDicom_openFxz.png
   :align: center


A left click on the first icon, or a selection of the ``import DICOM`` option of the ``File`` menu allows to start the selection of a folder containing all DICOM slices of one or several medical images of the patient. The DICOM reader is the latest ITK version developed by Kitware and used by a large set of medical software applications.

A left click on this icon, or a selection of the ``import data`` option of the ``File`` menu allows to load 3D modeled patient or exported DICOM images in two IRCAD format. The MFO format, as DICOM format, is a folder containing the 3D modelled patient data resulting of the VR-Anat software use. The Fxz is the specific VR-Planning format allowing to include in only one file one or several medical images and linked 3D models of one or several patients (see export data). This format will be also used to save resections.

When a patient is loaded, it is possible to visualize (or hide) the ``series selector`` windows containing the parameters of the medical image or the patient information by clicking on the ``info`` button on the tool bar :

.. image:: _static/View-INFO.png
   :align: center

The ``series selector`` window contains the name of the patient, the kind of modality (Ct-scan, MRI, etc…), the acquisition date, the image size, the voxel size and the comments. By clicking on the up left window icon, the ``property`` window is opened (or closed). It contains the patient’s birthdate, his gender and the hospital name. When data are anonymous, real patient information are erased and replaced by generic data. 

.. image:: _static/info-patient.png
   :align: center
