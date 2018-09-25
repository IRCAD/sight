# -*- coding: UTF8 -*-


import fnmatch
import os
import re
import sys

from collections import defaultdict
from functools   import partial
from subprocess  import Popen, PIPE

def p(x): print "==",x.name,"=="

usually_ignored = [
    'AGL',
    'AppKit',
    'ApplicationServices',
    'AudioToolbox',
    'AudioUnit',
    'Carbon',
    'Cocoa',
    'CoreAudio',
    'CoreFoundation',
    'CoreServices',
    'CoreVideo',
    'CoreWLAN',
    'Foundation',
    'IOKit',
    'OpenGL',
    'OpenCL',
    'QTKit',
    'QuartzCore',
    'QuickTime',
    'Security',
    'SystemConfiguration',
    'WebKit',
    'libSystem.B.dylib',
    'libgcc_s.1.dylib',
    'libiodbc',
    'libobjc.A.dylib',
    'libresolv',
    'libstdc++.6.dylib',
    ]


def cached_property(func):
    from functools import wraps
    name = func.__name__
    @wraps(func)
    def _get(self):
        try :
          return self.__dict__[name]
        except KeyError :
          value = func(self)
          self.__dict__[name]= value
          return value
    def _del(self):
        self.__dict__.pop(name, None)
    return property(_get, None, _del)

def try_remove_item(list_object):
    def remove(item):
        try:
            list_object.remove(item)
        except:
            pass
    return remove

class DepFilter(object):

    @staticmethod
    def dep_filter(dep):
        return all([
            dep,
            not '.o):' in dep,
            ])

    @staticmethod
    def no_executable_path_filter(dep):
            return (
                DepFilter.dep_filter(dep)
                and not dep.startswith('@executable_path')
             )


class BinaryFile(object):

    db = dict()
    ignored = defaultdict(set)
    fw_re = re.compile('.*?(?P<fw>[^/]+\.framework.+)')

    def __init__(self, path, dep_filter):
        self.path = path
        m = BinaryFile.fw_re.match(path)
        if m:
            self.name = m.groupdict()['fw']
        else:
            self.name = os.path.basename(path)
        self.dep_filter = dep_filter

    @cached_property
    def deps(self):
        otool = Popen(['otool', '-L', self.path], stdout=PIPE)
        output = otool.communicate()[0]
        binfiles = []
        if not otool.returncode:
            binfiles = [l.strip().split()[0] for l in output.splitlines()[1:]]
            binfiles = [BinaryFile(binfile, self.dep_filter)
                    for binfile in filter(self.dep_filter, binfiles) ]
            return dict((l.name, l) for l in binfiles)
        else:
            return {}

    def __repr__(self):
        return ''.join(['BinaryFile(path=', repr(self.path), ')'])

    def __str__(self):
        return ': '.join((self.name, self.path))

    def __eq__(self,other):
        return self.name == other.name

    def __hash__(self):
        return hash(self.name)

    @staticmethod
    def find(path, pattern, dep_filter):
        reg = fnmatch.translate(pattern)
        reg = re.compile(reg + '|[^.]+\Z')
        match = reg.match
        binfiles = []
        if os.path.isfile(path):
            binfiles += [BinaryFile(path, dep_filter)]
        else:
            for (root, dirs, files) in os.walk(path):
                map(try_remove_item(dirs), ['Headers','Resources'])
                files = [BinaryFile(os.path.join(root,f), dep_filter) for f in files]
                binfiles += [f for f in files if match(f.name)]
        return dict((l.name, l) for l in binfiles)


    def install_name_id(self, path_func):
        return [['-id', path_func(self.path)]]

    def install_name_changes(self, lib_set, path_func, ignored = None):
        deps = self.deps
        deps_set = set(deps.values())
        inter = lib_set.intersection(deps_set)
        if ignored is not None:
            ignored_libs = [lib.name for lib in deps_set.difference(lib_set)]
            ignored.update(ignored_libs)
            for ig in ignored_libs:
                BinaryFile.ignored[ig].add(self)
        res = [
                [ '-change', deps[lib.name].path,
                    path_func(self.db[lib.name].path) ] for lib in inter
              ]
        return res




def get_options(args):
    from optparse import OptionParser

    usage = "usage: %prog [options] binary_or_dir ..."
    epilog = """Updates the install names of executable or shared libraries
    found in argument list (binary_or_dir).
    """
    parser = OptionParser(usage=usage, epilog="")

    parser.add_option("-a", "--absolute-path",
                    action="store_true", dest="abs_path", default=False,
                    help=("Uses absolute path for install names that will be "
                          "relocated"),
                    )

    parser.add_option("-e", "--executable-path", dest="exec_path",
                    help=("specifies the executable path to which the install "
                          "names will be relocated. disabled if '-a' is used."),
                    metavar="DIR")

    #parser.add_option("-L", "--library-path",
    #                action="append", dest="libpath",
    #                help="libraries path", metavar="DIR")

    parser.add_option("-f", "--force",
                    action="store_true", dest="force", default=False,
                    help=("Updates dependency even if it already has an "
                          "@executable_path "),
                    )


    parser.add_option("-p", "--progress",
                  action="store_true", dest="progress", default=False,
                  help="show progress")

    parser.add_option("-i", "--show-ignored",
                  action="store_true", dest="show_ignored", default=False,
                  help="show libraries that have not been relocated")


    parser.add_option("-u", "--show-suspiciously-ignored",
                  action="count", dest="show_suspiciously_ignored",
                  default=False,
                  help="show libraries that have not been relocated "
                  "and should have. Set it twice to have more details on "
                  "wich libralibvtkVisuManagement.dylibries was requirering one of these.")


    parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="verbose output")

    parser.add_option("-P", "--search-pattern", dest="search_pattern",
            default="*.dylib", help= ("specifies the executable path to "
                        "which the install names will be relocated. "
                        "disabled if '-a' is used."), metavar="PATTERN")

    parser.add_option("-s", "--search",
                    action="append", dest="search_dirs", default=[],
                    help="this directory will be searched with pattern "
                    "provided by -P", metavar="DIR")



    (options, args) = parser.parse_args(args)
    return (options, args)


def main(options, args):
    def print_msg(*a):
        print ' '.join(map(str,a))
    def print_msg_n(*a):
        print ' '.join(map(str,a)),

    def mute(*a):
        pass

    verbose          = mute
    verbose_n        = mute
    progress_print   = mute
    progress_print_n = mute

    if options.verbose:
        verbose   = print_msg
        verbose_n = print_msg_n

    if options.progress:
        progress_print   = print_msg
        progress_print_n = print_msg_n


    if options.force:
        dep_filter = DepFilter.dep_filter
    else:
        dep_filter = DepFilter.no_executable_path_filter

    find_binary_files = partial(BinaryFile.find, dep_filter = dep_filter)
    update_db = lambda pattern : lambda path : BinaryFile.db.update(
                find_binary_files(path, pattern)
                )

    map(update_db('*.dylib'), args)
    map(update_db('*.framework/Versions/*'), args)
    map(update_db(options.search_pattern), options.search_dirs)

    relpath = os.path.relpath
    librelpath = lambda path : os.path.join( '@executable_path', relpath( path, options.exec_path ) )

    abspath = os.path.abspath
    libabspath = lambda path : abspath( path )

    if options.abs_path:
        install_name_func = libabspath
    else:
        install_name_func = librelpath


    ignored = set()
    L = len(BinaryFile.db)
    files = set(BinaryFile.db.values())

    for n,lib in enumerate(files):
        lib_file = lib.path
        progress_print_n(' ', '{0}/{1}'.format(n+1,L),'\r')
        sys.stdout.flush()
        if not os.path.islink(lib_file):
            changes = lib.install_name_id(install_name_func)
            changes += lib.install_name_changes(files, install_name_func, ignored)
            verbose(lib_file)
            for change in changes:
                os.chmod(lib_file, 0775)
                cmd = ['install_name_tool',] + change + [lib_file,]
                res = Popen(cmd, stdout=PIPE).communicate()[0]
                if res:
                    verbose(res)
    progress_print('')

    if any([
            options.show_ignored,
            options.show_suspiciously_ignored,
            ]):
        ignored = sorted( map(str,ignored))

    if options.show_ignored:
        print 'ignored libraries:', (os.linesep+'  ').join(ignored)

    if options.show_suspiciously_ignored:
        #suspicious = list(set(ignored) - set(usually_ignored))
        match = lambda x : not any( ig in x for ig in usually_ignored )
        suspicious = [ig for ig in ignored if match(ig)]
        libsep = (os.linesep+'  ')
        depsep = (os.linesep+'    - ')
        if options.show_suspiciously_ignored == 1:
            print 'suspiciously ignored :', libsep + libsep.join(suspicious)
        else:
            si = [["<%s>"%s + ', required by :']
                 + [l.name for l in BinaryFile.ignored[s]] for s in suspicious]
            print 'suspiciously ignored :', (libsep
                    + libsep.join(depsep.join(d) for d in si))


if __name__ == '__main__':
    (options, args) = get_options(sys.argv[1:])
    main(options, args)

