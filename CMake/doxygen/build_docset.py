#!/bin/python3
import bs4
import os
from pathlib import Path
import re
from shutil import rmtree, copyfile, copytree
import sqlite3
import sys

# Global regexes we don't want to recompile every single time we parse a file
class_file_re = re.compile('class([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
class_re = re.compile('fw4spl: (.+) Class Reference')
struct_file_re = re.compile('struct([a-zA-Z_][a-zA-Z0-9_]*)_1_1([a-zA-Z_][a-zA-Z0-9_]*)\.html')
struct_re = re.compile('fw4spl: (.+) Struct Reference')
srv_re = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*)::(S[A-Z][a-zA-Z0-9_]*) Class Reference')
obj_re = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*)::([A-Z][a-zA-Z0-9_]*) Class Reference')
iface_re = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*)::(I[A-Z][a-zA-Z0-9_]*|IService) Class Reference')
except_re = re.compile('fw4spl: ([a-zA-Z_][a-zA-Z0-9_]*)::([A-Z][a-zA-Z0-9_]*) Struct Reference')

# Regexes of the files to skip
file_skip_re = [
    re.compile('dir_.+\.html'),
    re.compile('.+_source.html'),
    re.compile('namespace.+\.html') # TODO: index namespaces
]

def bootstrap_docset():
    """
    Create the skeleton for the docset, i.e. the directory structure along with the SQLite database. Return the SQLite
    database connection.
    """
    # First, clear any remains from a previous docset build
    if Path('./Documentation/fw4spl.docset/').exists():
        rmtree('./Documentation/fw4spl.docset/')

    # Create the directory structure.
    Path('./Documentation/fw4spl.docset/Contents/Resources').mkdir(parents=True, exist_ok=True)

    # Then, create the SQLite database
    db = Path('./Documentation/fw4spl.docset/Contents/Resources/docSet.dsidx')
    if db.exists():
        os.remove(str(db))
    conn = sqlite3.connect(str(db))
    cur = conn.cursor()
    cur.execute('CREATE TABLE searchIndex(id INTEGER PRIMARY KEY, name TEXT, type TEXT, path TEXT);')
    cur.execute('CREATE UNIQUE INDEX anchor ON searchIndex (name, type, path);')
    conn.commit()
    return conn

def gather_sources():
    """
    Return a list containing the paths to all interesting HTML files contained at the root of the Doxygen html
    directory. We're not interested in what's in the subdirectories.
    """
    def recurse_dir(d):
        files = list()
        for _, _, dir_files in os.walk(d):
            files += [f for f in dir_files if f.endswith('.html')]
        for _d in subdirs:
            files += recurse_dir(_d)
        return files
    return recurse_dir('./Documentation/Doxygen/html')

def parse_file(f):
    """
    Parse a HTML file and return a (potentially empty) list of 3-tuples to add to the SQLite database.
    """
    # Doxygen names the documentation files in a friendly manner, which means we can guess what is inside from the file
    # name, and ignore files that we know we don't care about. This script currently looks for files containing classes
    # or structs.
    entries = list()
    if any(map(lambda regexp: regexp.match(f), file_skip_re)):
        return entries
    try:
        html = open(os.path.join('./Documentation/Doxygen/html', f), encoding="utf8").read()
        soup = bs4.BeautifulSoup(html, "html.parser")
        inherits_iservice = soup.find(class_='inherit_header pub_methods_classfwServices_1_1IService')
        inherits_object = soup.find(class_='inherit_header pub_methods_classfwData_1_1Object')
        inherits_exception = soup.find(class_='inherit_header pub_methods_classfwCore_1_1Exception')

        def is_service(f, soup):
            title = soup.title.get_text()
            match = srv_re.search(title)
            if match:
                bundle = match.group(1)
                srv = match.group(2)
                return (bundle + '::' + srv, "Service", f)

        def is_object(f, soup):
            title = soup.title.get_text()
            match = obj_re.search(title)
            if match:
                bundle = match.group(1)
                obj = match.group(2)
                return (bundle + '::' + obj, "Object", f)

        def is_interface(f, soup):
            title = soup.title.get_text()
            match = iface_re.search(title)
            if match:
                bundle = match.group(1)
                iface = match.group(2)
                return (bundle + '::' + iface, "Interface", f)

        def is_exception(f, soup):
            title = soup.title.get_text()
            match = except_re.search(title)
            if match:
                bundle = match.group(1)
                exception = match.group(2)
                return (bundle + '::' + exception, "Exception", f)

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

        if class_file_re.match(f):
            print("Class file: " + f)
            class_triple = file_class(f, soup)
            if class_triple is None:
                return entries
            class_name = class_triple[0]
            entries.append(class_triple)
            if inherits_iservice:
                triple = is_service(f, soup)
                if triple is not None:
                    entries.append(triple)
                triple = is_interface(f, soup)
                if triple is not None:
                    entries.append(triple)
            elif class_name == "fwData::Object":
                entries.append((class_name, "Object", f))
            elif inherits_object:
                triple = is_object(f, soup)
                if triple is not None:
                    entries.append(triple)
            elif class_name == "fwCore::Exception":
                entries.append((class_name, "Exception", f))
            elif inherits_exception:
                triple = is_exception(f, soup)
                if triple is not None:
                    entries.append(triple)
        elif struct_file_re.match(f):
            print("Struct file: " + f)
            struct_triple = file_struct(f, soup)
            if struct_triple is None:
                return entries
            struct_name = struct_triple[0]
            entries.append(struct_triple)
            if struct_name == "fwCore::Exception":
                entries.append((struct_name, "Exception", f))
            elif inherits_exception:
                triple = is_exception(f, soup)
                if triple is not None:
                    entries.append(triple)
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
    copytree('./Documentation/Doxygen/html', './Documentation/fw4spl.docset/Contents/Resources/Documents')

if __name__ == '__main__':
    conn = bootstrap_docset()
    html_files = gather_sources()
    entries = list()
    for f in html_files:
        new_entries = parse_file(f)
        if len(new_entries) != 0:
            print(repr(new_entries))
            entries += new_entries
    populate_db(conn, entries)
    copy_files()
    conn.close()
