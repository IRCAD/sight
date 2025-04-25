# sight::data::dicom

This directory contains a DICOM documentation parser (_dicom_parser.py_) and a DICOM class generator
(_class_generator.py_), written in Python. They are used to produce _Attribute.xxx_ and _Sop.xxx_ files, which hold
enums and information about all DICOM attributes (tags, VR, VM, ...) and all SOP classes. This allows friendlier
operations (no need to remember exact tag values, or SOP uids).

## DICOM parser

The parser _dicom_parser.py_ automatically downloads and reads the DICOM documentation. You can launch the parser
"standalone" like:

```python
python dicom_parser.py --sop-list "1.2.840.10008.5.1.4.1.1.6.2" --verbose
```

It will download all needed DICOM XML parts and parse them. Then it will display the list of IODs, modules, attributes
that belongs to SOP "Enhanced US Volume Storage".

It may serve to easily find which tags are present in which SOP, the module usage (mandatory, conditional, etc.), or the
attributes type. For example, to list all SOPs that use the attribute "FrameAcquisitionDateTime", you can use:

```python
python dicom_parser.py --mandatory-tags "(0018,9074)"
```

## DICOM class generator

The generator use the parser and share the same arguments to limit the list of SOPs / Attributes present in the
generated files. For example, the currently provided _Sop.xxx_ and _Attribute.xxx_ files are generated with:

```python
python class_generator.py --mandatory-tags "(7FE0,0010)" "(0062,0002)" "(0066,0002)" "(0066,0011)"
```

> The mandatory tag _(7FE0,0010)_ is the Pixel Data tag, which is present in all "Image" SOPs. The other tags
> _(0062,0002)_, _(0066,0002)_, _(0066,0011)_ are tags specific to surface/segmentation SOPs.