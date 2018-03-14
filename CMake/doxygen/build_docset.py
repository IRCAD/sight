#!/bin/python3
"""
Generate a dash docset from a doxygen documentation.
"""

import json
import os
from pathlib import Path
import re
import shutil
import sqlite3
import sys

import bs4

CFG = dict()
REPO_NAMES = dict()

# Global regexes we don't want to recompile every single time we parse a file
CLASS_FILE_RE = re.compile(r'class([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
CLASS_RE = re.compile('fw4spl: (.+) Class Reference')
STRUCT_FILE_RE = re.compile(r'struct([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
STRUCT_RE = re.compile('fw4spl: (.+) Struct Reference')
NAMESPACE_FILE_RE = re.compile(r'namespace.+\.html')
NAMESPACE_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_:]*) Namespace Reference')
SRV_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*(S[A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
BAD__SRV_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
OBJ_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Class Reference')
IFACE_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*(I[A-Z0-9][a-zA-Z0-9_]*|IService)) Class Reference')
EXCEPT_RE = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*::(?:[a-zA-Z_][a-zA-Z0-9_]*::)*([A-Z0-9][a-zA-Z0-9_]*)) Struct Reference')

# Regexes of the files to skip
FILE_SKIP_RE = [
    re.compile('pages.html'),
    re.compile(r'dir_.+\.html'),
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
    conn_ = sqlite3.connect(str(db))
    cur = conn_.cursor()
    cur.execute('CREATE TABLE searchIndex(id INTEGER PRIMARY KEY, name TEXT, type TEXT, path TEXT);')
    cur.execute('CREATE UNIQUE INDEX anchor ON searchIndex (name, type, path);')
    conn_.commit()
    return conn_

def parse_json_config():
    """
    Parse the projects.json configuration file. It is expected to be present in the working directory.
    """

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
    return pages

# TODO: strip the leading repository path from the HTML file to get rid of the ugly full paths ?
def file_repo(f_soup):
    """
    Return the name of the repository that a particular documentation file was generated from, or None if not possible.
    """
    lists = f_soup.find_all('ul')
    if lists:
        file_path = lists[-1].li.get_text()
        for repo in CFG['repositories']:
            candidates = [repo for repo in CFG['repositories'] if file_path.startswith(repo)]
            if candidates:
                res = max(candidates, key=len)
                return REPO_NAMES[res]
    return None

def parse_file(f_):
    """
    Parse a HTML file and return a (potentially empty) list of 3-tuples to add to the SQLite database.
    """
    # Doxygen names the documentation files in a friendly manner, which means we can guess what is inside from the file
    # name, and ignore files that we know we don't care about. This script currently looks for files containing classes
    # or structs.
    new_entries = list()
    # Some files are of no interest to us and can be skipped
    if any(map(lambda regexp: regexp.match(f_), FILE_SKIP_RE)):
        return new_entries
    try:
        html = open(os.path.join('./html', f_), encoding="utf8").read()
        soup = bs4.BeautifulSoup(html, "html.parser")
        inherits_iservice = soup.find(class_='inherit_header pub_methods_classfwServices_1_1IService')
        inherits_object = soup.find(class_='inherit_header pub_methods_classfwData_1_1Object')
        inherits_exception = soup.find(class_='inherit_header pub_methods_classfwCore_1_1Exception')

        item_type_re = {
            "Service": SRV_RE,
            "Object": OBJ_RE,
            "Interface": IFACE_RE,
            "Exception": EXCEPT_RE
        }
        def is_item_type(soup, ty_str):
            """
            Test if the HTML contained in the supplied soup describes and element of the specified type based on the
            doxygen page title. Accepted types are 'Service', 'Object', 'Interface' and 'Exception'. If true, return an
            entry to add to the sqlite DB, else return None.
            """
            title = soup.title.get_text()
            match = item_type_re[ty_str].search(title)
            if match:
                path = match.group(1)
                repo = file_repo(soup)
                if repo is not None:
                    path = path + " ({})".format(repo)
                return (path, ty_str, f_)
            return None

        def is_bad_service(soup):
            """
            Test if the HTML contained in the supplied soup describes a service, with more lenient rules regarding
            the name of the service. If true, print a warning regarding the service name and return an entry to add to
            the sqlite DB, otherwise return None.
            """
            title = soup.title.get_text()
            match = BAD__SRV_RE.search(title)
            if match:
                path = match.group(1)
                srv = match.group(2)
                repo = file_repo(soup)
                if repo is not None:
                    path = path + " ({})".format(repo)
                print("Warning: service {} has non compliant name (no S prefix)".format(srv))
                return (path, "Service", f_)
            return None

        file_type_re = {
            "Class": CLASS_RE,
            "Namespace": NAMESPACE_RE,
            "Struct": STRUCT_RE,
        }
        def is_file_type(soup, ty_str):
            """
            Test if the HTML contained in the supplied soup describes and element of the specified type based on the
            doxygen page title. Accepted types are 'Class', 'Namespace', and 'Struct'. If true, return an
            entry to add to the sqlite DB, else return None.
            """
            title = soup.title.get_text()
            match = file_type_re[ty_str].search(title)
            if match:
                struct_ = match.group(1)
                return (struct_, ty_str, f_)
            return None

        if CLASS_FILE_RE.match(f_):
            # We know the file contains a class, find what kind of class
            class_triple = is_file_type(soup, 'Class')
            if class_triple is None:
                return new_entries
            class_name = class_triple[0]
            if inherits_iservice:
                # The class inherits IService, it can be a service or an interface
                triple = is_item_type(soup, 'Interface')
                if triple is not None:
                    new_entries.append(triple)
                else:
                    # Not an interface, probably a service
                    triple = is_item_type(soup, 'Service')
                    if triple is not None:
                        new_entries.append(triple)
                    else:
                        triple = is_bad_service(soup)
                        if triple is not None:
                            new_entries.append(triple)
                        else:
                            print("Warning: unexepected behaviour for class {} while parsing file {}".format(class_name, f_))
            elif class_name == "fwData::Object":
                # Special case, Object is not an actual data.
                new_entries.append((class_name, "Class", f_))
            elif inherits_object:
                # Not a service and inherits fwData::Object, this class is probably a data.
                triple = is_item_type(soup, 'Object')
                if triple is not None:
                    new_entries.append(triple)
            elif class_name == "fwCore::Exception":
                # Special case for fwCore::Exception
                new_entries.append((class_name, "Exception", f_))
            elif inherits_exception:
                # Inherits an exception type, this is probably an exception
                # TODO: I'm pretty sure this won't catch all exceptions in the codebase
                triple = is_item_type(soup, 'Exception')
                if triple is not None:
                    new_entries.append(triple)
            else:
                # Plain old class
                new_entries.append(class_triple)
        elif STRUCT_FILE_RE.match(f_):
            # We know the file contains a struct, find what kind of struct
            struct_triple = is_file_type(soup, 'Struct')
            if struct_triple is None:
                return new_entries
            new_entries.append(struct_triple)
            if inherits_exception:
                # Inherits an exception type, this is probably an exception
                # TODO: I'm pretty sure this won't catch all exceptions in the codebase
                triple = is_item_type(soup, 'Exception')
                if triple is not None:
                    new_entries.append(triple)
        elif NAMESPACE_FILE_RE.match(f_):
            # We know the file contains a namespace, find what kind of namespace (i.e. Bundle, Library, regular
            # namespace...)
            namespace_triple = is_file_type(soup, 'Namespace')
            if namespace_triple is None:
                return new_entries
            namespace_name = namespace_triple[0]
            if namespace_name in CFG['srclibs']:
                new_entries.append((namespace_name, "Library", f_))
            elif namespace_name in CFG['bundles']:
                # There is no 'Bundle' entry type, unfortunately. Component, Package or Module would be suitable
                # replacements. I chose Package.
                new_entries.append((namespace_name, "Package", f_))
            else:
                new_entries.append(namespace_triple)
    except UnicodeDecodeError:
        print('The file ' + f_ + ' is not valid UTF-8')
    except FileNotFoundError:
        # Occurs for files in the search subdirectory, it's OK, we don't care about those
        pass
    return new_entries

def populate_db(conn_, services):
    """
    Fill the sqlite database with the supplied list of (name, entry_type, file_path) triples.
    """
    cur = conn_.cursor()
    for triple in services:
        try:
            cur.execute("INSERT OR IGNORE INTO searchIndex(name, type, path) VALUES (?, ?, ?);", triple)
        except Exception:
            print("Error inserting " + str(triple))
    conn_.commit()

def copy_files():
    """
    Copy the doxygen HTML files into the docset destination.
    """
    try:
        shutil.copytree('./html', './fw4spl.docset/Contents/Resources/Documents')
    except shutil.Error as err:
        errors = err.args[0]
        print("Warning: some files were not copied correctly. The generated docset might be incomplete.")
        for src, _, why in errors:
            print("File '" + src + "' was not copied correctly. Reason: " + why)

def main():
    """
    Builds the dash docset.
    """
    global CFG
    global REPO_NAMES
    try:
        CFG = json.loads(open('./projects.json', encoding="utf8").read())
    except (OSError, json.JSONDecodeError) as err:
        print("Error loading configuration file: " + str(err))
        return
    CFG = parse_json_config()
    REPO_NAMES = {repo: Path(repo).parent.name if Path(repo).name == "src" else Path(repo).name for repo in CFG['repositories']}
    if CFG is None:
        sys.exit(1)
    conn = bootstrap_docset()
    html_files = gather_sources()
    entries = list()
    for f in html_files:
        f_entries = parse_file(f)
        if f_entries:
            entries += f_entries
    entries += parse_related_pages()
    populate_db(conn, entries)
    copy_files()
    conn.close()

if __name__ == '__main__':
    main()
