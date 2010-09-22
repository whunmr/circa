#!/usr/bin/env python

import os

def mkdir(dir):
    if os.path.exists(dir):
        return
    (parent, _) = os.path.split(dir)
    mkdir(parent)
    os.path.mkdir(dir)
    
def read_text_file(path):
    if not os.path.exists(path):
        return ""
    f = open(path, 'r')
    return f.read()[:-1]

def write_text_file(path, contents):
    # read the file first, and only write if the contents are different. This saves
    # us from triggering a rebuild on build systems that check the modified time.
    if contents == read_text_file(path):
        return

    f = open(path, 'w')
    f.write(contents)
    f.write("\n")
    
def get_cpp_files_in_dir(dir):
    files = []
    for file in os.listdir(dir):
        if file.endswith('.cpp'):
            files.append(file)
    return files

def get_cpp_file_names(dir):
    return map(lambda s: s[:-4], get_cpp_files_in_dir(dir))

def setup_builtin_functions():
    dir = 'src/functions'

    namespaces = map(lambda s: s+'_function', get_cpp_file_names(dir))
    function_decls = '\n'.join(
            sorted(map(lambda n: 'namespace '+n+' { void setup(Branch& kernel); }', namespaces)))
    function_calls = '\n    '.join(
            sorted(map(lambda n: n+'::setup(kernel);', namespaces)))

    return """
// Copyright (c) 2007-2010 Paul Hodge. All rights reserved.

// This file is generated during the build process by prebuild.py .
// You should probably not edit this file manually.

#include "common_headers.h"

#include "branch.h"

namespace circa {

%s

void setup_builtin_functions(Branch& kernel)
{
    %s
}

} // namespace circa""" % (function_decls, function_calls)
# end of setup_builtin_functions
    
def register_all_tests():
    dir = 'src/tests'
    #print "cpp file names = " + str(get_cpp_file_names(dir))

    namespaces = get_cpp_file_names(dir)
    function_decls = '\n'.join(
            sorted(map(lambda n: 'namespace '+n+' { void register_tests(); }', namespaces)))
    function_calls = '\n    '.join(
            sorted(map(lambda n: n+'::register_tests();', namespaces)))

    return """\
// Copyright (c) 2007-2010 Paul Hodge. All rights reserved.

// This file is generated during the build process by prebuild.py .
// You should probably not edit this file manually.

#include "common_headers.h"

#include "testing.h"

namespace circa {

%s

void register_all_tests()
{
    gTestCases.clear();

    %s
}

} // namespace circa""" % (function_decls, function_calls)
# end of register_all_tests

def text_file_to_c_string(sourceFile, variableName):
    source = open(sourceFile)
    out = []

    out.append("// This file was autogenerated from "+sourceFile)
    out.append("")
    out.append("namespace circa {")
    out.append("")
    out.append('extern "C" const char* '+variableName+" = ")

    def escape_line(line):
        out = []
        for c in line:
            if c == '"': out.append('\\"')
            elif c == '\\': out.append('\\\\')
            else: out.append(c)
        return "".join(out)

    while source:
        line = source.readline()
        if line == "": break
        line = line[:-1]
        line = escape_line(line)
        out.append('    "' + line + '\\n"')

    out[-1] += ";"
    out.append("")
    out.append("} // namespace circa")
    out.append("")

    return "\n".join(out)

mkdir('src/generated')

# generate setup_builtin_functions.cpp and register_all_tests.cpp
write_text_file('src/generated/setup_builtin_functions.cpp', setup_builtin_functions())
write_text_file('src/generated/register_all_tests.cpp', register_all_tests())

# generate builtin_script_text.cpp
write_text_file('src/generated/builtin_script_text.cpp',
        text_file_to_c_string("src/ca/builtins.ca", "BUILTIN_SCRIPT_TEXT"))

# generate all_tests.cpp, all_builtin_functions.cpp, and all_builtin_types.cpp
def source_files(dir):
    for path in os.listdir(dir):
        if not os.path.isfile(os.path.join(dir,path)): continue
        if not path.endswith('.cpp'): continue
        yield path
def builtin_function_cpps():
    for file in get_cpp_files_in_dir('src/functions'):
        yield "functions/"+file
def test_cpps():
    for file in get_cpp_files_in_dir('src/tests'):
        yield "tests/"+file
def library_sources():
    for file in get_cpp_files_in_dir('src'):
        if file == 'main.cpp': continue
        yield file
def include_list(items):
    generated_cpp = []
    for item in items:
        generated_cpp.append('#include "'+item+'"')
    return "\n".join(generated_cpp)

write_text_file('src/generated/all_tests.cpp', include_list(test_cpps()))
write_text_file('src/generated/all_builtin_functions.cpp',
    include_list(builtin_function_cpps()))
write_text_file('src/generated/all_builtin_types.cpp',
    include_list(['types/'+file for file in source_files('src/types')]))
