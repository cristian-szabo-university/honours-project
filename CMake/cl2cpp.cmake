if(NOT EXISTS "${CL_DIR}")
    message (FATAL_ERROR "Specified wrong OpenCL kernels directory: ${CL_DIR}")
endif()

file(GLOB cl_list "${CL_DIR}/*.cl" )
list(SORT cl_list)

if(NOT cl_list)
    message (FATAL_ERROR "Can't find OpenCL kernels in directory: ${CL_DIR}")
endif()

string(REPLACE ".cpp" ".hpp" FILE_HPP_OUT "${FILE_OUT}")
get_filename_component(FILE_HPP_OUT_NAME "${FILE_HPP_OUT}" NAME)

set(STR_CPP 
"// This file is auto-generated. Do not edit!
#include \"${FILE_HPP_OUT_NAME}\"

namespace HonoursProject
{
    ProgramEntry::ProgramEntry(const std::string & name, const std::string & data, const std::string & hash)
        : name(name), data(data), hash(hash)
    {
    }

    const std::string & ProgramEntry::getName() const
    {
        return name;
    }

    const std::string & ProgramEntry::getData() const
    {
        return data;
    }

    const std::string & ProgramEntry::getHash() const
    {
        return hash;
    }
")

set(STR_HPP 
"// This file is auto-generated. Do not edit!
#pragma once

#include <string>

namespace HonoursProject
{
    class ProgramEntry
    {
    public:

        ProgramEntry(const std::string& name, const std::string& data, const std::string& hash);

        const std::string& getName() const;

        const std::string& getData() const;

        const std::string& getHash() const;

    private:

        std::string name;

        std::string data;

        std::string hash;

    };
")

foreach(cl ${cl_list})
    get_filename_component(cl_filename "${cl}" NAME_WE)
    file(READ "${cl}" lines)

    string(REPLACE "\r" "" lines "${lines}\n")
    string(REPLACE "\t" "  " lines "${lines}")

    string(REGEX REPLACE "/\\*([^*]/|\\*[^/]|[^*/])*\\*/" ""   lines "${lines}") # multiline comments
    string(REGEX REPLACE "/\\*([^\n])*\\*/"               ""   lines "${lines}") # single-line comments
    string(REGEX REPLACE "[ ]*//[^\n]*\n"                 "\n" lines "${lines}") # single-line comments
    string(REGEX REPLACE "\n[ ]*(\n[ ]*)*"                "\n" lines "${lines}") # empty lines & leading whitespace
    string(REGEX REPLACE "^\n"                            ""   lines "${lines}") # leading new line

    string(REPLACE "\\" "\\\\" lines "${lines}")
    string(REPLACE "\"" "\\\"" lines "${lines}")
    string(REPLACE "\n" "\\n\"\n\"" lines "${lines}")

    string(REGEX REPLACE "\"$" "" lines "${lines}") # unneeded " at the eof

    string(MD5 hash "${lines}")

    set(STR_CPP_DECL "\n    const ProgramEntry ${cl_filename}\n    (\n    \"${cl_filename}\",\n    \"${lines},\n    \"${hash}\");\n\n")
    
    set(STR_HPP_DECL "\n    extern const ProgramEntry ${cl_filename};\n")

    set(STR_CPP "${STR_CPP}${STR_CPP_DECL}")
    set(STR_HPP "${STR_HPP}${STR_HPP_DECL}")
endforeach()

set(STR_CPP "${STR_CPP}}")

set(STR_HPP "${STR_HPP}}")

file(WRITE "${DIR_OUT}/Source/${FILE_OUT}" "${STR_CPP}")

if(EXISTS "${DIR_OUT}/Include/${FILE_HPP_OUT}")
    file (READ "${DIR_OUT}/Include/${FILE_HPP_OUT}" hpp_lines)
endif()

if("${hpp_lines}" STREQUAL "${STR_HPP}")
    message (STATUS "${FILE_HPP_OUT} contains same content")
else()
    file(WRITE "${DIR_OUT}/Include/${FILE_HPP_OUT}" "${STR_HPP}")
endif()