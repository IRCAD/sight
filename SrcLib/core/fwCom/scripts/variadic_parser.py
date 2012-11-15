# -*- coding: UTF8 -*-


import collections
import os
import re

from pyparsing import Word, alphas, OneOrMore, Literal, Optional, \
        delimitedList, Group, nestedExpr, Forward, CharsNotIn, \
        originalTextFor, printables, White

try:
    import watchdog
except:
    watchdog = None

def flatten(l):
    for el in l:
        if isinstance(el, collections.Iterable) and not isinstance(el, basestring):
            for sub in flatten(el):
                yield sub
        else:
            yield el



def parse_variadic_templates(txt):
    template_param_type = Word(alphas)
    template_variadic = Literal('...')
    template_id = Word(alphas)

    template_variadic_param = Group(
            template_param_type + template_variadic + template_id
            )

    template_param = Group( template_param_type + template_id )

    # template_params = Group ( delimitedList( template_variadic_param | Optional(template_param) ) )
    template_params = (
            Optional( OneOrMore(template_param + ',') )
            + template_variadic_param
            + Optional( OneOrMore( ',' + template_param ) )
            )

    template_params_no_variadic = (
            template_param + Optional( OneOrMore( ',' + template_param ) )
            )

    template_decl = Optional(
            "template"
            + Literal("<") + template_params_no_variadic +  Literal(">")
            ) + "template" + Literal("<") + template_params +  Literal(">")

    block_content = Forward()
    block = nestedExpr('{', '}', content=block_content) + Literal(';') * (0,1)
    block_content <<  ( CharsNotIn('{}') | block )

    decl = originalTextFor( template_decl + CharsNotIn('{') + block )


    template_file = Forward()
    code_block = decl | White() | Word(printables)
    template_file << ( Optional(OneOrMore(code_block)) | template_file)


    parsed = template_file.parseString( txt )

    return parsed


def unpack_variadic_template(tpl, dim, max_dim=-1):

    comma_reg = "(?P<comma>,?) *"
    comma_sub = "" if dim == 0 else "\g<comma> "

    comma_end_reg = "(?P<comma_end>,?) *"
    comma_end_sub = "" if dim == 0 else "\g<comma_end> "

    variadic_declaration = '(?P<var_decl_type>(typename|class)) *\.\.\. *(?P<var_decl_id>\w+)'

    void = 'boost::mpl::void_'

    subs = []
    if max_dim == -1:
        max_dim = dim

    dim_diff = max_dim-dim

    is_tpl_decl = False

    # Add mpl::void_ elements to simulate variadic templates structs/classes
    template_decl_reg = ("(?P<decl_part_1>template *<(?P<decl_args>(?P<pre_var_args>[^.>]*)"
            + variadic_declaration +
            "(?P<post_var_args>[^>.]*))>\s*(struct|class) *\w+)"
                         "(?P<decl_part_2>\s*[^<:{]*\s*[:{])")


    if dim < max_dim:
        def template_decl_sub(m):
            spec_args = ''.join(
                        (
                        m.group('pre_var_args'),
                        )

                        + ( (
                        m.group('var_decl_id'), '... ',', '
                        ) if dim != 0 else tuple() )

                        +(
                        ', '.join( (void, ) * dim_diff ),

                        m.group('post_var_args'),
                        )
                    )

            spec_args = spec_args.replace('typename','')
            spec_args = spec_args.replace('class','')

            sub = ''
            sub += m.group('decl_part_1')

            sub += '<' + spec_args + '>'
            sub += m.group('decl_part_2') + ' '
            return sub
    else:
        if re.search(template_decl_reg, tpl):
            is_tpl_decl = True

        def template_decl_sub(m):
            sub = ''
            part1 = m.group('decl_part_1')
            part1 = re.sub(variadic_declaration, '\g<0> = '+ void, part1)
            sub += part1
            sub += m.group('decl_part_2') + ' '
            return sub



    r = range(1, dim+1)

    variadic_declaration_reg = variadic_declaration
    variadic_declaration_sub = '\g<var_decl_type> \g<var_decl_id>{0}'

    if is_tpl_decl:
        # need to match added default values
        variadic_declaration_reg += '(?P<has_def_value> *=? *)(?P<def_value>[^>,)]+)'
        variadic_declaration_sub += '\g<has_def_value>\g<def_value>'


#comma for T... or t...
    def variadic_expand(m):
        sub = ''
        if dim == 0 :
            if m.group('comma') and m.group('comma_end'):
                sub = ', '
        else:
            sub += m.group('comma') + ' '
            sub += ', '.join((m.group('id')+"{0}").format(i) for i in r)
            sub += m.group('comma_end') + ' '
        return sub



    subs += [

# Add mpl::void_ elements to simulate variadic templates structs/classes
            (template_decl_reg, template_decl_sub),

#variadic declaration ( ...T )
            (variadic_declaration_reg,
                ', '.join(variadic_declaration_sub.format(i) for i in r)),

#variadic sizeof
            ('sizeof\.\.\.\(\w+\)', str(dim)),

#function apply ( some_func(a)... )
            ('(?P<func>\w+) *\((?P<id>\w+)\) *\.\.\.',
                ', '.join("\g<func>( \g<id>{0} )".format(i) for i in r)),

# T...t
            ('(?P<qual>(const *)?)(?P<type>\w+)(?P<qual2> *[&*]*) *\.\.\. *(?P<arg>\w+)',
                ', '.join("\g<qual>\g<type>{0}\g<qual2> \g<arg>{0}".format(i) for i in r)),

# T... or t...
            # ('(?P<id>\w+) *\.\.\. *(?P<end_comma>,?)',
            ('(?P<id>\w+) *\.\.\.',
                variadic_expand
                # ', '.join("\g<id>{0}".format(i) for i in r)
                ),

    ]


    for reg, sub in subs:
        if isinstance(sub, basestring):
            tpl = re.sub(
                    comma_reg + reg + comma_end_reg,
                    comma_sub + sub + comma_end_sub,
                    tpl)
        else:
            tpl = re.sub(
                    # reg,
                    comma_reg + reg + comma_end_reg,
                    sub, tpl )


    # keep template<> for struct & classes, remove others
    def fix_bad_template(m):
        sub = m.group('g1')
        if re.search('(\\bstruct\\b|\\bclass\\b)', sub):
            sub = m.expand('\g<0>')
        return sub

    tpl = re.sub('template *< *>(?P<g1>\s*.+)', fix_bad_template, tpl)

    #Fixes bad empty '<>'. To protect goods '<>' inside blocks, use typedefs
    def fix_bad_chevrons(m):
        sub = m.group('part1')
        if 'typedef' in sub:
            sub += '<>'
        sub += m.group('part2')
        return sub

    tpl = re.sub('(?P<part1>\s*(?!typedef).*(?<!template) *)< *>(?P<part2>[^{;]*;)', fix_bad_chevrons, tpl)

    return tpl



def get_unpacked_content( f, nb_args ):

    txt = open(f,'r').read()


    output = []

    append = output.append

    for p in parse_variadic_templates(txt):
        if '...' in p:
            append( '' + '\n' )
            append( '#ifdef BOOST_NO_VARIADIC_TEMPLATES' + '\n' )
            append( '//' + '='*79 + '\n' )
            append( '//' + '='*79 + '\n' )
            append( '//' + '='*36 + ' BEGIN ' + '='*36 + '\n' )
            for i in reversed(range(nb_args+1)):
                append( (unpack_variadic_template(p,i,nb_args)) + '\n' )
            append( '//' + '='*37 + ' END ' + '='*37 + '\n' )
            append( '//' + '='*79 + '\n' )
            append( '//' + '='*79 + '\n' )
            append( '' + '\n' )
            append( '#else  // BOOST_NO_VARIADIC_TEMPLATES' + '\n' )
            append(p + '\n' )
            append( '#endif  // BOOST_NO_VARIADIC_TEMPLATES' + '\n' )
        else:
            append( p )

    unpacked = ''.join(output)

    return re.sub(' +$', '', unpacked, 0, re.M)







def process_one_file(input_path, unpack_num, output_path = None):

    result = get_unpacked_content(input_path, unpack_num)

    if output_path:
        open(output_path, 'wb').write(result)
    else:
        print result


def process_dir(input_path, unpack_num, extension, force = False):
    fname_end = '.' + extension
    offset = 1+len(extension)
    for dirpath, dirnames, filenames in os.walk(input_path):
        for fname in filenames:
            if fname.endswith(fname_end):
                file_in = os.path.join(dirpath, fname)
                file_out = os.path.join(dirpath, fname[:-offset])
                t_in, t_out = 0,0
                try:
                    t_in = os.path.getmtime(file_in)
                    t_out = os.path.getmtime(file_out)
                except OSError:
                    pass
                if force or t_in >= t_out:
                    print 'processing', file_in, '...'
                    process_one_file(file_in, unpack_num, file_out)




def main():

    import argparse

    desc = '''Aproximative processing of variadic templates.
Generate a file emulating variadic templates with given number of argument.
'''

    parser = argparse.ArgumentParser( description=desc )

    parser.add_argument('input_path', metavar='PATH', type=str,
                    help='c++ file or dir to process')
    parser.add_argument('-n', dest='num', type=int, default=9,
            help='Max number of argument to emulate')

    parser.add_argument('-o', dest='output_file', type=str, default=None,
            help='output file')

    parser.add_argument('-e', dest='extension', type=str, default='vrdc',
            help='extension of files to parse')

    parser.add_argument('-f', dest='force', action='store_true',
            help='force file processing in directory')


    if watchdog:
        parser.add_argument('--watch','-w', action='store_true',
                help='watch mode : run %(prog)s as daemon, watching changes '
                'in specified file/dir')

    options = parser.parse_args()

    path_is_dir = os.path.isdir(options.input_path)

    if path_is_dir:
        if options.output_file is not None :
            parser.error("-o is incompatible with directory processing")
        process_dir(
                options.input_path,
                options.num,
                options.extension,
                options.force
                )
    else:
        process_one_file(options.input_path, options.num, options.output_file)

    if options.watch:
        if path_is_dir:
            print 'watching "{0}" ...'.format( options.input_path )
        from watchdog.observers import Observer
        from watchdog.events import PatternMatchingEventHandler
        import Queue

        events = Queue.Queue()

        import signal


        def keyboard_interrupt_handler(signal, frame):
            print 'exiting ...'
            keyboard_interrupt_handler.interupted = True

        keyboard_interrupt_handler.interupted = False

        signal.signal(signal.SIGINT, keyboard_interrupt_handler)

        class ModifiedEventHandler(PatternMatchingEventHandler):

            def __init__(self, extension):
                super(ModifiedEventHandler, self).__init__(
                        ['*.' + extension],
                        ignore_directories=True
                        )

                self.extension = extension
                self.offset = 1+len(extension)

            def process(self, file_path):
                output_path = file_path[:-self.offset]
                events.put( (file_path, options.num, output_path) )

            def on_created(self, event):
                self.process(event.src_path)

            def on_moved(self, event):
                if event.dest_path.endswith(self.extension):
                    self.process(event.dest_path)

            def on_modified(self, event):
                self.process(event.src_path)


        event_handler = ModifiedEventHandler(options.extension)
        observer = Observer()
        observer.schedule(
                event_handler,
                path=options.input_path,
                recursive=True
                )
        observer.start()

        count = 0
        while not keyboard_interrupt_handler.interupted:
            try:
                toProcess = events.get(True, 0.1)
                count += 1
                print '[{0}]'.format(count),'processing', \
                        toProcess[0], '=>', toProcess[2]
                process_one_file(*toProcess)
                print '[{0}]'.format(count),'done'
            except Queue.Empty:
                pass

        observer.stop()
        observer.join()



if __name__ == '__main__':
    main()

else:
    import difflib
    import sys
    import unittest

    class TestSequenceFunctions(unittest.TestCase):

        def test_get_unpacked_content(self):
            reference = open('test/unpacked.cpp').read()
            result = get_unpacked_content('test/variadic.cpp', 9);


            reference = reference.splitlines(True)
            result = result.splitlines(True)

            diff = ''.join(difflib.unified_diff(reference, result))

            no_diff = '' == diff

            if not no_diff:
                sys.stderr.write(diff)

            self.assertTrue(no_diff)



