#!/bin/python3
import bs4
from enum import Enum
import json
import os
from pathlib import Path
import re
import shutil
import sqlite3
import sys

# Global regexes we don't want to recompile every single time we parse a file
class_file_re     = re.compile('class([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
class_re          = re.compile('fw4spl: (.+) Class Reference')
struct_file_re    = re.compile('struct([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
struct_re         = re.compile('fw4spl: (.+) Struct Reference')
namespace_file_re = re.compile('namespace.+\.html')
namespace_re      = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_:]*) Namespace Reference')
srv_re            = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*(S[A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
bad_srv_re        = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
obj_re            = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
iface_re          = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*(I[A-Z0-9][a-zA-Z0-9_]*|IService)) Class Reference')
except_re         = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Struct Reference')

# Regexes of the files to skip
file_skip_re = [
    re.compile('pages.html'),
    re.compile('dir_.+\.html'),
    re.compile('.+_source.html')
]

def bootstrap_docset():
    """
    Create the skeleton for the docset, i.e. the directory structure along with the SQLite database. Return the SQLite
    database connection.
    """
    # Create the directory structure.
    Path('./fw4spl.docset/Contents/Resources').mkdir(parents=True, exist_ok=True)

    # Then, create the SQLite database
    db = Path('./fw4spl.docset/Contents/Resources/docSet.dsidx')
    if db.exists():
        os.remove(str(db))
    conn = sqlite3.connect(str(db))
    cur = conn.cursor()
    cur.execute('CREATE TABLE searchIndex(id INTEGER PRIMARY KEY, name TEXT, type TEXT, path TEXT);')
    cur.execute('CREATE UNIQUE INDEX anchor ON searchIndex (name, type, path);')
    conn.commit()
    return conn

def parse_json_config():
    """
    Parse the projects.json configuration file. It is expected to be present in the working directory.
    """
    try:
        cfg = json.loads(open('./projects.json', encoding="utf8").read())
    except (OSError, json.JSONDecodeError) as err:
        print("Error loading configuration file: " + str(err))
        cfg = None
    return cfg

def gather_sources():
    """
    Return a list containing the paths to all interesting HTML files contained at the root of the Doxygen html
    directory. We're not interested in what's in the subdirectories.
    """
    files = list()
    for _, _, dir_files in os.walk('./html/'):
        files += [f for f in dir_files if f.endswith('.html')]
    return files

def parse_related_pages():
    """
    Parse the 'pages.html' doxygen file and generate the list of related pages.
    """
    pages = list()
    html = open(os.path.join('./html', 'pages.html'), encoding="utf8").read()
    soup = bs4.BeautifulSoup(html, "html.parser")
    table = soup.find("table", class_="directory")
    for cell in table.find_all("tr"):
        page_name = cell.td.a.string
        page_link = cell.td.a.get('href')
        pages.append((page_name, "Guide", page_link))
        print("Guide: {}@{}".format(page_name, page_link))
    return pages

def parse_file(f):
    """
    Parse a HTML file and return a (potentially empty) list of 3-tuples to add to the SQLite database.
    """
    # Doxygen names the documentation files in a friendly manner, which means we can guess what is inside from the file
    # name, and ignore files that we know we don't care about. This script currently looks for files containing classes
    # or structs.
    entries = list()
    # Some files are of no interest to us and can be skipped
    if any(map(lambda regexp: regexp.match(f), file_skip_re)):
        return entries
    try:
        html = open(os.path.join('./html', f), encoding="utf8").read()
        soup = bs4.BeautifulSoup(html, "html.parser")
        inherits_iservice = soup.find(class_='inherit_header pub_methods_classfwServices_1_1IService')
        inherits_object = soup.find(class_='inherit_header pub_methods_classfwData_1_1Object')
        inherits_exception = soup.find(class_='inherit_header pub_methods_classfwCore_1_1Exception')

        def is_service(f, soup):
            title = soup.title.get_text()
            match = srv_re.search(title)
            if match:
                path = match.group(1)
                srv = match.group(2)
                return (path, "Service", f)

        def is_bad_service(f, soup):
            title = soup.title.get_text()
            match = bad_srv_re.search(title)
            if match:
                path = match.group(1)
                srv = match.group(2)
                print("Warning: service {} has non compliant name (no S prefix)".format(srv))
                return (path, "Service", f)

        def is_object(f, soup):
            title = soup.title.get_text()
            match = obj_re.search(title)
            if match:
                path = match.group(1)
                obj = match.group(2)
                return (path, "Object", f)

        def is_interface(f, soup):
            title = soup.title.get_text()
            match = iface_re.search(title)
            if match:
                path = match.group(1)
                iface = match.group(2)
                return (path, "Interface", f)

        def is_exception(f, soup):
            title = soup.title.get_text()
            match = except_re.search(title)
            if match:
                path = match.group(1)
                exception = match.group(2)
                return (path, "Exception", f)

        def file_class(f, soup):
            title = soup.title.get_text()
            match = class_re.search(title)
            if match:
                class_ = match.group(1)
                return (class_, "Class", f)

        def file_struct(f, soup):
            title = soup.title.get_text()
            match = struct_re.search(title)
            if match:
                struct_ = match.group(1)
                return (struct_, "Struct", f)

        def file_namespace(f, soup):
            title = soup.title.get_text()
            match = namespace_re.search(title)
            if match:
                namespace = match.group(1)
                return (namespace, "Namespace", f)

        if class_file_re.match(f):
            # We know the file contains a class, find what kind of class
            class_triple = file_class(f, soup)
            if class_triple is None:
                return entries
            class_name = class_triple[0]
            if inherits_iservice:
                # The class inherits IService, it can be a service or an interface
                triple = is_interface(f, soup)
                if triple is not None:
                    entries.append(triple)
                else:
                    # Not an interface, probably a service
                    triple = is_service(f, soup)
                    if triple is not None:
                        entries.append(triple)
                    else:
                        triple = is_bad_service(f, soup)
                        if triple is not None:
                            entries.append(triple)
                        else:
                            print("Warning: unexepected behaviour for class {} while parsing file {}".format(class_name, f))
            elif class_name == "fwData::Object":
                # Special case, Object is not an actual data.
                entries.append((class_name, "Class", f))
            elif inherits_object:
                # Not a service and inherits fwData::Object, this class is probably a data.
                triple = is_object(f, soup)
                if triple is not None:
                    entries.append(triple)
            elif class_name == "fwCore::Exception":
                # Special case for fwCore::Exception
                entries.append((class_name, "Exception", f))
            elif inherits_exception:
                # Inherits an exception type, this is probably an exception
                # TODO: I'm pretty sure this won't catch all exceptions in the codebase
                triple = is_exception(f, soup)
                if triple is not None:
                    entries.append(triple)
            else:
                # Plain old class
                entries.append(class_triple)
        elif struct_file_re.match(f):
            # We know the file contains a struct, find what kind of struct
            struct_triple = file_struct(f, soup)
            if struct_triple is None:
                return entries
            struct_name = struct_triple[0]
            entries.append(struct_triple)
            if inherits_exception:
                # Inherits an exception type, this is probably an exception
                # TODO: I'm pretty sure this won't catch all exceptions in the codebase
                triple = is_exception(f, soup)
                if triple is not None:
                    entries.append(triple)
        elif namespace_file_re.match(f):
            # We know the file contains a namespace, find what kind of namespace (i.e. Bundle, Library, regular
            # namespace...)
            namespace_triple = file_namespace(f, soup)
            if namespace_triple is None:
                return entries
            namespace_name = namespace_triple[0]
            if namespace_name in cfg['srclibs']:
                entries.append((namespace_name, "Library", f))
            elif namespace_name in cfg['bundles']:
                # There is no 'Bundle' entry type, unfortunately. Component, Package or Module would be suitable
                # replacements. I chose Package.
                entries.append((namespace_name, "Package", f))
            else:
                entries.append(namespace_triple)
    except UnicodeDecodeError:
        print('The file ' + f + ' is not valid UTF-8')
    except FileNotFoundError:
        # Occurs for files in the search subdirectory, it's OK, we don't care about those
        pass
    return entries

def populate_db(conn, services):
    cur = conn.cursor()
    for triple in services:
        try:
            cur.execute("INSERT OR IGNORE INTO searchIndex(name, type, path) VALUES (?, ?, ?);", triple)
        except ValueError:
            print("Error inserting " + str(triple))
    conn.commit()

def copy_files():
    try:
        shutil.copytree('./html', './fw4spl.docset/Contents/Resources/Documents')
    except shutil.Error as err:
        errors = err.args[0]
        print("Warning: some files were not copied correctly. The generated docset might be incomplete.")
        for src, dst, why in errors:
            print("File '" + src + "' was not copied correctly. Reason: " + why)

if __name__ == '__main__':
    global cfg
    cfg = parse_json_config()
    if cfg is None:
        sys.exit(1)
    conn = bootstrap_docset()
    html_files = gather_sources()
    entries = list()
    for f in html_files:
        new_entries = parse_file(f)
        if len(new_entries) != 0:
            entries += new_entries
    entries += parse_related_pages()
    populate_db(conn, entries)
    copy_files()
    conn.close()
