import argparse
import fnmatch
import os
import re
import sys
import xml.etree.ElementTree
from dataclasses import dataclass
from urllib.request import urlretrieve


# cspell: ignore Wundefined afterloader angio autorefraction autosequence boli bscan cadsr ciexy ctdi ctdivol decf
# cspell: ignore deidentification dermoscopic diconde dicos dvhroi exif fiducials fluence fnumber frameof gpsdop hicpp
# cspell: ignore hpgl imager inms inua inus iods isop ivol ivus keratoconus keratometer keratometric kerma lensometry
# cspell: ignore lookat meterset metersets mrfov multiway nmpet nmtomo nolintbegin nolintend octb octz oecf olink
# cspell: ignore postamble powerline prcs presaturation radiofluoroscopic relaxivity reprojection rgblut rtdvh rtroi
# cspell: ignore sddn sdhn sdvn softcopy soundpath stereometric stowrs tlhc tomo tomosynthesis tomotherapeutic toric
# cspell: ignore tractography xaxrf

@dataclass(eq=True, order=True, frozen=True)
class Tag:
    group: str
    element: str


@dataclass(eq=True, order=True, frozen=True)
class Attribute:
    tag: Tag
    name: str
    keyword: str
    vr: str
    vm: str
    description: str


@dataclass(eq=True, order=True, frozen=True)
class AttributeElement:
    attribute: Attribute
    type: str
    description: str
    attribute_elements: list
    origin: str


@dataclass(eq=True, order=True, frozen=True)
class Module:
    name: str
    keyword: str
    attribute_elements: list[AttributeElement]


@dataclass(eq=True, order=True, frozen=True)
class ModuleElement:
    module: Module
    usage: str
    condition: str


@dataclass(eq=True, order=True, frozen=True)
class Iod:
    name: str
    keyword: str
    module_elements: list[ModuleElement]
    functional_group_elements: list[ModuleElement]


@dataclass(eq=True, order=True, frozen=True)
class Uid:
    value: str
    name: str
    keyword: str
    type: str


@dataclass(eq=True, order=True, frozen=True)
class Sop:
    uid: Uid
    iod: Iod


# Program arguments
g_parser = None
g_arguments = None
g_source_url = None
g_mandatory_tags = list[Tag]()
g_sop_list = list[str]()

# Global XML namespaces
g_namespaces = {
    "xml": "http://www.w3.org/XML/1998/namespace",
    "docbook": "http://docbook.org/ns/docbook"
}

g_invalid = "INVALID"

# To avoid recompiling regexes
g_snake_regex1 = re.compile(r"((?<=[a-z0-9])[A-Z]|(?!^)[A-Z](?=[a-z]))")
g_keyword_regex = re.compile(r"[\W_]+")
g_tag_regex = re.compile(r"\(\s*([0-9A-Fx]{4})\s*,\s*([0-9A-Fx]{4})\s*\)")
g_uid_regex = re.compile(r"(0|[1-9][0-9]*)(\.(0|[1-9][0-9]*))*")
g_uid_wildcard_regex = re.compile(r"[0-9.*?]+")

# Various cache to speedup parsing
g_documents_cache = dict[str, xml.etree.ElementTree.ElementTree]()
g_iods_cache = dict[str, Iod]()
g_modules_cache = dict[str, Module]()
g_attribute_elements_cache = dict[str, list[AttributeElement]]()

# Parsing results
g_attributes = dict[Tag, Attribute]()
g_vrs = set[str]()
g_vms = set[str]()
g_uids = dict[str, Uid]()
g_uid_types = set[str]()
g_filtered_sops = dict[Uid, Sop]()
g_filtered_iods = dict[str, Iod]()
g_filtered_modules = dict[str, Module]()
g_filtered_attributes = dict[Tag, Attribute]()
g_filtered_attribute_types = set[str]()


def check_mandatory_tags(sop: Sop) -> bool:
    """Check if a SOP is matching the tag filters"""

    # Check if we have filters
    if g_mandatory_tags is None or len(g_mandatory_tags) == 0:
        return True

    # Check if the SOP has a matching tag
    iod = sop.iod

    def check_module_element(a_module_element: ModuleElement) -> bool:
        module = a_module_element.module
        if module is not None and module.attribute_elements is not None:

            def check_attribute_elements(attribute_elements: list[AttributeElement], path: list[str]) -> bool:
                for attribute_element in attribute_elements:
                    if attribute_element.attribute.tag in g_mandatory_tags:
                        return True

                    # Check sub-attribute elements
                    sub_attribute_elements = attribute_element.attribute_elements
                    if (
                            sub_attribute_elements is not None
                            and len(sub_attribute_elements) > 0
                            and attribute_element.origin not in path
                    ):
                        # Avoid infinite recursion
                        new_path = path + [attribute_element.origin]

                        # Recurse
                        if check_attribute_elements(sub_attribute_elements, new_path):
                            return True
                return False

            if check_attribute_elements(module.attribute_elements, []):
                return True

        return False

    for module_element in iod.module_elements:
        if check_module_element(module_element):
            return True

    for module_element in iod.functional_group_elements:
        if check_module_element(module_element):
            return True

    return False


def open_document(part_name: str) -> xml.etree.ElementTree:
    """Open a document and cache it"""

    # Check the cache
    if part_name in g_documents_cache:
        return g_documents_cache[part_name]

    # Download the file if needed
    filename = f"{part_name}.xml"
    if not os.path.isfile(filename):
        url = f"{g_source_url}/{part_name}/{filename}"
        print(f"Downloading {url}...")
        urlretrieve(url, filename)

    print(f"Opening {filename}...")
    with open(file=filename, mode="r", encoding="utf-8") as document_file:
        document = xml.etree.ElementTree.parse(document_file)
        g_documents_cache[part_name] = document
        return document


def to_keyword(name: str) -> str:
    """Convert a name to a PascalCase"""
    # Remove any non-alphanumeric character
    result = g_keyword_regex.sub('_', name)

    # Capitalise each word (But keep already uppercase characters)
    return "".join(word[0].upper() + word[1:] for word in result.split('_'))


def xml_element_to_string(element: xml.etree.ElementTree.Element) -> str:
    """Convert an element to a string"""
    emphasis = element.find("./docbook:emphasis", g_namespaces)

    if emphasis is not None:
        element = emphasis

    string = element.text or ""
    string = string.replace(u"\u200b", "").replace(u"\u00b5", "u")
    return string.strip()


def xml_element_to_tag(element: xml.etree.ElementTree.Element) -> Tag:
    """Convert a tag element to a group and element string"""

    # Get the tag string
    tag_string = xml_element_to_string(element)

    # Clean the tag
    tag_string = tag_string.replace("(", "").replace(")", "").replace("x", "0")

    # Split the group and the element
    split_tag = tag_string.split(",")

    return Tag(
        group=split_tag[0].strip(),
        element=split_tag[1].strip()
    )


def xml_element_to_vr(element: xml.etree.ElementTree.Element) -> str:
    """Convert a vr element to a vr string"""

    # Get the vr string
    vr_string = xml_element_to_string(element)

    # If there is no vr, return "INVALID"
    if len(vr_string) == 0 or vr_string.find("See") != -1:
        return g_invalid

    # If the vr is a list, split and reconstruct it
    split_vr = [v.strip() for v in vr_string.split(" or ")]
    joined_vr = "_".join(sorted(split_vr))

    return joined_vr


def xml_element_to_vm(element: xml.etree.ElementTree.Element) -> str:
    """Convert a vm element to a vm string"""

    # Get the vm string
    vm_string = xml_element_to_string(element).lower()

    # If there is no vm, return "INVALID"
    if len(vm_string) == 0:
        return g_invalid

    # If the vm can be a range or 1, take the range
    if vm_string.find("1-n or 1") != -1:
        vm_string = "1-n"

    # Normalize all vm as ranges
    split_vm = vm_string.split("-")
    vm = "MIN_" + split_vm[0] + "_MAX_"

    if len(split_vm) >= 2:
        vm += split_vm[1]
    else:
        vm += split_vm[0]

    return vm.upper()


def xml_element_to_usage(element: xml.etree.ElementTree.Element) -> tuple[str, str]:
    """Convert a usage element to a "usage" / "condition" string pair"""
    pair = xml_element_to_string(element).split("-")
    if len(pair) == 1:
        return pair[0].strip(), ""
    elif len(pair) > 2:
        return pair[0].strip(), "-".join(pair[1:]).strip()
    else:
        return pair[0].strip(), pair[1].strip()


def xml_element_to_uid(element: xml.etree.ElementTree.Element) -> str:
    """Convert an element to a uid"""
    uid = xml_element_to_string(element)

    if g_uid_regex.match(uid) is None:
        raise ValueError(f"Invalid UID: {uid}")

    return uid


def parse_attributes() -> tuple[dict[Tag, Attribute], set[str], set[str]]:
    """Parse attributes from DICOM docbook files"""

    # List all source files and their tables
    sources = [
        ("part07", {"table_E.1-1"}),
        ("part06", {"table_7-1", "table_8-1", "table_6-1"})
    ]

    attributes = dict[Tag, Attribute]()
    vrs = set[str]()
    vms = set[str]()

    # For each source files
    for source in sources:
        document = open_document(source[0])

        # For each table
        for table_id in source[1]:

            # Find the table
            table = document.find(f".//*[@xml:id=\"{table_id}\"]", g_namespaces)

            for row in table.iterfind("./docbook:tbody/docbook:tr", g_namespaces):
                columns = row.findall("./docbook:td/docbook:para", g_namespaces)

                if len(columns) > 5:
                    description = xml_element_to_string(columns[5])

                    # Do not add retired attributes
                    if description[0:3].upper() == "RET":
                        continue
                else:
                    description = ""

                tag = xml_element_to_tag(columns[0])
                name = xml_element_to_string(columns[1])
                keyword = xml_element_to_string(columns[2])
                vr = xml_element_to_vr(columns[3])
                vm = xml_element_to_vm(columns[4])

                attributes[tag] = Attribute(
                    tag=tag,
                    name=name,
                    keyword=keyword,
                    vr=vr,
                    vm=vm,
                    description=description
                )

                if vr != g_invalid:
                    vrs.add(vr)

                if vm != g_invalid:
                    vms.add(vm)

    return attributes, vrs, vms


def find_sibling_attribute_elements(level_string: str, root_list: list[AttributeElement]) -> list[AttributeElement]:
    """Get the nested list of data elements"""

    # Find the level
    if level_string is not None:
        level = level_string.count(">")
    else:
        level = 0

    # If we are "nested"
    if level > 0:
        # Get the right data element
        nested_attribute_element = root_list[-1]
        level -= 1

        while level > 0:
            nested_attribute_element = nested_attribute_element.attribute_elements[-1]
            level -= 1

        return nested_attribute_element.attribute_elements
    else:
        return root_list


def is_tag(element: xml.etree.ElementTree.Element) -> bool:
    """Check if an element is a tag"""
    string = xml_element_to_string(element)

    if string is None or len(string) == 0:
        return False

    return g_tag_regex.match(string) is not None


def tag_as_argument(string: str) -> Tag:
    """Return a tag as an argument"""
    match = g_tag_regex.match(string)

    if match is None:
        raise ValueError(f"Invalid tag: {string}")

    return Tag(match.group(1), match.group(2))


def sop_uid_as_argument(string: str) -> str:
    """Return a tag as an argument"""
    if string is None or len(string) == 0:
        raise ValueError("Invalid SOP UID")

    string = string.strip()

    if len(string) > 64:
        raise ValueError(f"SOP UID too long: {string}")

    match = g_uid_regex.match(string)

    if match is None:
        wildcard_match = g_uid_wildcard_regex.match(string)

        if wildcard_match is None:
            raise ValueError(f"Invalid SOP UID: {string}")

    return string


def is_attribute_type(element: xml.etree.ElementTree.Element) -> bool:
    """Check if an element is a type"""
    attribute_type = xml_element_to_string(element)

    if attribute_type is None or len(attribute_type) == 0:
        return False

    if (
            attribute_type != "1"
            and attribute_type != "1C"
            and attribute_type != "2"
            and attribute_type != "2C"
            and attribute_type != "3"
    ):
        return False

    return True


def is_module_usage(element: xml.etree.ElementTree.Element) -> bool:
    """Check if an element is an usage"""
    usage = xml_element_to_string(element)

    if usage is None or len(usage) == 0:
        return False

    if (
            usage != "M"
            and usage != "U"
            and not usage.startswith("C - ")
    ):
        return False

    return True


def fetch_attributes(table_id: str) -> list[AttributeElement]:
    """fetch all data element from link recursively"""

    if table_id in g_attribute_elements_cache:
        return g_attribute_elements_cache[table_id]

    part3 = open_document("part03")

    attribute_elements = list[AttributeElement]()

    # Find the table
    print(f"Opening {table_id}...")
    table = part3.find(f".//*[@xml:id=\"{table_id}\"]", g_namespaces)

    # For each row
    for row in table.iterfind("./docbook:tbody/docbook:tr", g_namespaces):

        # Get all columns
        columns = row.findall("./docbook:td/docbook:para", g_namespaces)

        # Get the attribute name to have the nested level and to look for special "any attribute" cases
        name = xml_element_to_string(columns[0])

        # Check for tag first because it is easier to discriminate links
        tag = None
        attribute_type = None

        # Get the tag and the type, if any.
        # Since there are "spanning", the index may vary and make the parsing more complex.
        # Thus, we simply try to guess if the current column look like a "Tag" or a type column
        for column in row.iterfind("./docbook:td/docbook:para", g_namespaces):
            if is_tag(column):
                tag = xml_element_to_tag(column)

            if is_attribute_type(column):
                attribute_type = xml_element_to_string(column)

        # Special case for "any attribute"
        if tag is None and re.match(r".*any.*attribute.*", name.lower()) is not None:
            tag = Tag("xxxx", "xxxx")

        if tag is not None:

            # Find the attribute
            attribute = g_attributes[tag]

            if len(columns) >= 4:
                description = xml_element_to_string(columns[3])
            elif len(columns) >= 3:
                description = xml_element_to_string(columns[2])
            else:
                description = ""

            if attribute_type is not None:
                # This row is most likely a data element without type
                attribute_element = AttributeElement(
                    attribute=attribute,
                    type=attribute_type,
                    description=description,
                    attribute_elements=list[AttributeElement](),
                    origin=table_id

                )
            else:
                # This row is most likely a data element without type
                attribute_element = AttributeElement(
                    attribute=attribute,
                    type=g_invalid,
                    description=description,
                    attribute_elements=list[AttributeElement](),
                    origin=table_id
                )

            # Find where we must add the new element
            sibling_attribute_elements = find_sibling_attribute_elements(name, attribute_elements)
            sibling_attribute_elements.append(attribute_element)

            # Add to the cache for later use and manage circular references
            g_attribute_elements_cache[table_id] = attribute_elements

        else:

            # Try to find a link
            xref = None
            emphasis = None

            # Do it per column because of some malformed tables where xref element is a sibling not a child of emphasis
            for column in row.iterfind("./docbook:td/docbook:para", g_namespaces):
                if xref is None:
                    xref = column.find(".//docbook:xref", g_namespaces)

                if emphasis is None:
                    emphasis = column.find(".//docbook:emphasis", g_namespaces)

            if xref is not None:
                # Get the linked table
                linked_table = xref.attrib["linkend"]

                # Retrieve the linked data elements
                linked_elements = fetch_attributes(linked_table)

                # Find where we must add the linked macro
                sibling_attribute_elements = find_sibling_attribute_elements(emphasis.text, attribute_elements)
                sibling_attribute_elements.extend(linked_elements)

                # Add to the cache for later use and manage circular references
                g_attribute_elements_cache[table_id] = attribute_elements

            # Exclusion for special cases (comments, Functional Groups, etc.)
            elif name == "BASIC CODED ENTRY ATTRIBUTES":
                continue
            elif name == "ENHANCED ENCODING MODE":
                continue
            elif len(attribute_elements) > 0 and (
                    attribute_elements[-1].attribute.tag == Tag("5200", "9229")
                    or attribute_elements[-1].attribute.tag == Tag("5200", "9230")
            ):
                # We ignore Functional Groups for now since we need to replace them by the IOD specific ones
                continue
            else:
                print(f"Unable to find xref or tag in table {table_id}")

    return attribute_elements


def fetch_module(module_section_id: str) -> Module:
    """Fetch a module"""

    # If already fetched, return it
    if module_section_id in g_modules_cache:
        return g_modules_cache[module_section_id]

    part3 = open_document("part03")

    # Find the section
    section = part3.find(f".//*[@xml:id=\"{module_section_id}\"]", g_namespaces)

    # Find the section title for the name
    name = section.find("./docbook:title", g_namespaces).text

    print(f"Fetching Module {name}...")

    attribute_elements = list[AttributeElement]()

    # Find the module attributes table
    for attributes_table in section.iterfind(".//docbook:table", g_namespaces):
        caption = attributes_table.find("./docbook:caption", g_namespaces)

        # Eliminate unwanted tables
        if caption is None:
            continue

        if caption.text is None:
            continue

        lower_caption = caption.text.lower()

        if lower_caption.find("example") != -1 or (
                lower_caption.find("module attributes") == -1
                and lower_caption.find("macro attributes") == -1
                and lower_caption.find("module table") == -1
        ):
            continue

        # Get the table id
        attributes_table_id = attributes_table.attrib["{http://www.w3.org/XML/1998/namespace}id"]

        # Fetch the attributes
        attribute_elements.extend(fetch_attributes(attributes_table_id))

    if len(attribute_elements) == 0:
        raise Exception(f"Unable to find any attributes in {name}")

    module = Module(
        name=name,
        keyword=to_keyword(name),
        attribute_elements=attribute_elements
    )

    g_modules_cache[module_section_id] = module

    return module


def fetch_module_usages(iod_modules_table: xml.etree.ElementTree.Element) -> list[ModuleElement]:
    """Build ModuleUsage list from a modules table"""
    module_usages = list[ModuleElement]()

    # For each row get the referenced module
    for row in iod_modules_table.iterfind("./docbook:tbody/docbook:tr", g_namespaces):
        # Get the linked module
        module_link = row.find("./docbook:td/docbook:para/docbook:xref", g_namespaces)

        # Workaround for malformed XML
        if module_link is None:
            module_link = row.find("./docbook:td/docbook:xref", g_namespaces)

        module_section_id = module_link.attrib["linkend"]

        usage = "M"
        condition = ""

        # Get the usage, if any. The index may vary and make the parsing more complex
        # Thus, we simply try to guess if the current column look like an "Usage" column
        for column in row.iterfind("./docbook:td/docbook:para", g_namespaces):
            if is_module_usage(column):
                usage, condition = xml_element_to_usage(column)
                break

        module_usage = ModuleElement(
            module=fetch_module(module_section_id),
            usage=usage,
            condition=condition
        )

        module_usages.append(module_usage)

    if len(module_usages) == 0:
        raise Exception(f"Unable to find any module in table.")

    return module_usages


def fetch_iod(iod_section_id: str) -> Iod:
    """Fetch an IOD from a section"""
    # Look first in the cache
    if iod_section_id in g_iods_cache:
        return g_iods_cache[iod_section_id]

    part3 = open_document("part03")

    # Find the section
    iod_section = part3.find(f".//*[@xml:id=\"{iod_section_id}\"]", g_namespaces)

    # Find the section title for the name
    name = iod_section.find("./docbook:title", g_namespaces).text

    print(f"Fetching IOD {name}...")

    modules = list[ModuleElement]()
    functional_groups = list[ModuleElement]()

    # Find the IOD modules table
    for iod_modules_table in iod_section.iterfind(".//docbook:table", g_namespaces):
        caption = iod_modules_table.find("./docbook:caption", g_namespaces)

        # Eliminate unwanted tables
        if caption is None:
            continue

        if caption.text is None:
            continue

        lower_caption = caption.text.lower()

        if lower_caption.find("example") != -1:
            continue

        if lower_caption.find("iod modules") != -1:
            # Add regular modules to the IOD
            modules.extend(fetch_module_usages(iod_modules_table))
        elif lower_caption.find("group macros") != -1:
            # The IOD section contains a table with Functional Groups, add them
            functional_groups.extend(fetch_module_usages(iod_modules_table))

    if len(modules) == 0:
        raise Exception(f"Unable to find any module in {name}")

    iod = Iod(
        name=name,
        keyword=to_keyword(name),
        module_elements=modules,
        functional_group_elements=functional_groups
    )

    # Store in cache for later use
    g_iods_cache[iod_section_id] = iod

    return iod


def parse_sop_classes() -> dict[Uid, Sop]:
    """Parse SOP classes from DICOM docbook files"""
    sop_classes = dict[Uid, Sop]()

    part4 = open_document("part04")

    # For each tables
    for table_id in {
        "table_B.5-1",
        "table_I.4-1",
        "table_GG.3-1",
        "table_KK.1-2"
    }:
        # Find the table
        table = part4.find(f".//*[@xml:id=\"{table_id}\"]", g_namespaces)

        # For each rows
        for row in table.iterfind("./docbook:tbody/docbook:tr", g_namespaces):

            # Read all columns
            columns = row.findall("./docbook:td/docbook:para", g_namespaces)

            if len(columns) >= 3:

                uid = g_uids[xml_element_to_string(columns[1])]

                # Here we have to look if we need to filter this uid
                if g_sop_list is not None:

                    keep = False

                    for sop_pattern in g_sop_list:
                        if fnmatch.fnmatch(uid.value, sop_pattern):
                            keep = True

                    if not keep:
                        continue

                # Column 3 is a link to IOD in part 3, get it
                section_link = columns[2].find("./docbook:olink", g_namespaces)

                # Workaround for malformed XML
                if section_link is None:
                    section_link = row.find("./docbook:td/docbook:olink", g_namespaces)

                # Follow the link...
                section_id = section_link.attrib["targetptr"]
                iod = fetch_iod(section_id)

                sop = Sop(
                    uid=uid,
                    iod=iod
                )

                # Check if all tags is present
                if check_mandatory_tags(sop):
                    print(f"Adding SOP {sop.uid.name}...")
                    sop_classes[sop.uid] = sop

    return sop_classes


def parse_uids() -> tuple[dict[str, Uid], set[str]]:
    """Parse UIDs from DICOM docbook files"""
    uids = dict[str, Uid]()
    uid_types = set[str]()

    part6 = open_document("part06")

    # Find the section
    uids_table = part6.find(f".//*[@xml:id=\"table_A-1\"]", g_namespaces)
    for row in uids_table.iterfind("./docbook:tbody/docbook:tr", g_namespaces):
        columns = row.findall("./docbook:td/docbook:para", g_namespaces)

        name = xml_element_to_string(columns[1])

        # Do not add retired UIDs
        if name.lower().find("retired") != -1:
            continue

        uid_type = xml_element_to_string(columns[3])
        uid_types.add(uid_type)

        uid = Uid(
            value=xml_element_to_uid(columns[0]),
            name=name,
            keyword=xml_element_to_string(columns[2]),
            type=uid_type
        )

        uids[uid.value] = uid

    return uids, uid_types


def print_sop(sop: Sop):
    """Print a SOP class"""
    print(f"{sop.uid.name}")
    print(f"├─UID: {sop.uid.value}")
    print(f"├─Keyword: {sop.uid.keyword}")
    print(f"└─{sop.iod.name}")
    print(f"  ├─IOD keyword: {sop.iod.keyword}")
    print(f"  └─IOD modules ({len(sop.iod.module_elements)}):")

    for module_usage_index, module_usage in enumerate(sop.iod.module_elements):

        line = f"{module_usage.module.name} ({module_usage.usage}):"

        is_last_module = module_usage_index >= len(sop.iod.module_elements) - 1
        if not is_last_module:
            print(f"    ├─{line}")
        else:
            print(f"    └─{line}")

        def print_attribute_elements(tree: str, attribute_elements: list[AttributeElement], path: list[str]):

            for attribute_element_index, attribute_element in enumerate(attribute_elements):

                is_functional_group = attribute_element.attribute.tag == Tag(
                    group="5200",
                    element="9229"
                ) or attribute_element.attribute.tag == Tag(
                    group="5200",
                    element="9230"
                )

                attribute_element_line = f"({attribute_element.attribute.tag.group}, {attribute_element.attribute.tag.element}) ({attribute_element.type}) {attribute_element.attribute.name}"

                if attribute_element.attribute_elements is not None and len(attribute_element.attribute_elements) > 0:
                    attribute_element_line += f" ({len(attribute_element.attribute_elements)}):"

                is_last_element = attribute_element_index >= len(attribute_elements) - 1
                if not is_last_element:
                    print(tree + f"├─{attribute_element_line}")
                else:
                    print(tree + f"└─{attribute_element_line}")

                if (
                        attribute_element.attribute_elements is not None
                        and len(attribute_element.attribute_elements) > 0
                        and attribute_element.origin not in path
                ):
                    # This is for breaking circular references
                    new_path = path + [attribute_element.origin]

                    if is_last_element and not is_functional_group:
                        print_attribute_elements(tree + "   ", attribute_element.attribute_elements, new_path)
                    else:
                        print_attribute_elements(tree + "│  ", attribute_element.attribute_elements, new_path)

                if is_functional_group:
                    if is_last_element:
                        tree += "   "
                    else:
                        tree += "│  "

                    for functional_groups_index, functional_groups_usage in enumerate(
                            sop.iod.functional_group_elements):
                        functional_line = f"{functional_groups_usage.module.name} ({functional_groups_usage.usage}):"
                        if functional_groups_index < len(sop.iod.functional_group_elements) - 1:
                            print(tree + f"├─{functional_line}")
                        else:
                            print(tree + f"└─{functional_line}")

                        if (
                                functional_groups_usage.module.attribute_elements is not None
                                and len(functional_groups_usage.module.attribute_elements) > 0
                        ):

                            # This is for breaking circular references
                            new_path = path + [attribute_element.origin]

                            if is_last_element:
                                print_attribute_elements(
                                    tree + "   ", functional_groups_usage.module.attribute_elements, new_path
                                )
                            else:
                                print_attribute_elements(
                                    tree + "│  ", functional_groups_usage.module.attribute_elements, new_path
                                )

        if is_last_module:
            print_attribute_elements("       ", module_usage.module.attribute_elements, [])
        else:
            print_attribute_elements("    │  ", module_usage.module.attribute_elements, [])

    print()


def main():
    """Main function"""

    # Parse arguments
    global g_parser
    g_parser = argparse.ArgumentParser("Generate DICOM dictionary files")

    g_parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Print verbose output"
    )

    g_parser.add_argument(
        "source_url",
        nargs="?",
        default="http://dicom.nema.org/medical/dicom/current/source/docbook",
        help="Source URL of the DICOM docbook files"
    )

    g_parser.add_argument(
        "--sop-list",
        nargs="+",
        help="Only search for given SOP classes",
        dest="sop_list",
        type=sop_uid_as_argument
    )

    g_parser.add_argument(
        "--mandatory-tags",
        nargs="+",
        help="Only add SOP classes which contain the given tags",
        dest="mandatory_tags",
        type=tag_as_argument
    )

    global g_arguments
    g_arguments = g_parser.parse_args()

    global g_sop_list
    g_sop_list = g_arguments.sop_list

    global g_mandatory_tags
    g_mandatory_tags = g_arguments.mandatory_tags

    global g_source_url
    g_source_url = g_arguments.source_url

    # Parse Attributes
    global g_attributes, g_vrs, g_vms
    g_attributes, g_vrs, g_vms = parse_attributes()

    # Add Special case "any" attribute
    g_attributes[Tag("xxxx", "xxxx")] = Attribute(
        tag=Tag("xxxx", "xxxx"),
        name="Any",
        keyword="Any",
        vr="UNDEFINED",
        vm="UNDEFINED",
        description=""
    )

    # Parse UIDs
    global g_uids, g_uid_types
    g_uids, g_uid_types = parse_uids()

    # Parse SOPs
    global g_filtered_sops
    g_filtered_sops = parse_sop_classes()

    # Filter results
    global g_filtered_iods
    global g_filtered_modules
    global g_filtered_attributes
    global g_filtered_attribute_types

    for sop in g_filtered_sops.values():

        # Add used IODs
        g_filtered_iods[sop.iod.keyword] = sop.iod

        # Recursively add used attributes
        def walk_for_attributes(attribute_elements: list[AttributeElement], path: list[str]):
            for attribute_element in attribute_elements:

                # Add used attribute
                g_filtered_attributes[attribute_element.attribute.tag] = attribute_element.attribute

                # Add used attribute type
                g_filtered_attribute_types.add(attribute_element.type)

                # Recurse through sub-elements
                if (
                        attribute_element.attribute_elements is not None
                        and len(attribute_element.attribute_elements) > 0
                        and attribute_element.origin not in path
                ):
                    # The "path" allows breaking circular references
                    new_path = path + [attribute_element.origin]
                    walk_for_attributes(attribute_element.attribute_elements, new_path)

        for module_element in sop.iod.module_elements:
            # Add used module
            g_filtered_modules[module_element.module.keyword] = module_element.module

            # Walk recursively through data elements
            walk_for_attributes(module_element.module.attribute_elements, [])

        if sop.iod.functional_group_elements is not None:
            for module_element in sop.iod.functional_group_elements:
                # Add used module
                g_filtered_modules[module_element.module.keyword] = module_element.module

                # Walk recursively through data elements
                walk_for_attributes(module_element.module.attribute_elements, [])

    print("\n\nParsing results:")
    print("_" * 120)

    # Display results:
    if g_sop_list is not None:
        print(f"Filtering by SOP Class UID: {str(g_sop_list)}.")

    if g_mandatory_tags is not None:
        print(f"Filtering by mandatory tags: {str(g_mandatory_tags)}.")

    print("_" * 120)

    print(f"\nFound {len(g_filtered_sops)} SOP classes:")
    for sop in g_filtered_sops.values():
        print(f"\t{sop.uid.value}: {sop.uid.name}")

    print("_" * 120)

    print(f"\nFound {len(g_filtered_iods)} IODs:")
    for iod in g_filtered_iods.values():
        print(f"\t{iod.name}")

    print("_" * 120)

    print(f"\nFound {len(g_filtered_modules)} Modules:")
    for module in g_filtered_modules.values():
        print(f"\t{module.name}")

    print("_" * 120)

    print(f"\nFound {len(g_filtered_attributes)} Attributes:")
    for attribute in g_filtered_attributes.values():
        print(f"\t({attribute.tag.group}, {attribute.tag.element}): {attribute.name}")

    print("_" * 120)

    if g_arguments.verbose:
        print(f"\nSOP classes details:\n")

        # Print SOP classes details
        for sop in g_filtered_sops.values():
            print_sop(sop)
            print("_" * 120)

    return 0


if __name__ == "__main__":
    sys.exit(main())
