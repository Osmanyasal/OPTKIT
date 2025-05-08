#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 22:34:56 2023

@author: osman.yasal

Purpose
===========

Read all <vendor>_<pmu>_events.h files in libpfm4 and convert them to 
compatible c++ class higherarchy for effective usage.

example layout is as follows:

-- icl.hh --

#include <cstdint>
#include "src/intel_priv.hh"
namespace optkit::intel::icl{
	enum icl : uint64_t {
		UNHALTED_CORE_CYCLES = 0x3c, // Count core clock cycles whenever the clock signal on the specific core is running (not halted)
		UNHALTED_REFERENCE_CYCLES = 0x0300, // Unhalted reference cycles
		INSTRUCTION_RETIRED = 0xc0, // Number of instructions at retirement
		INSTRUCTIONS_RETIRED = 0xc0, // Number of instructions at retirement
		SQ_MISC = 0x00f4, // SuperQueue miscellaneous.
        ...
        ...
        ...
    };
};

namespace icl = optkit::intel::icl;

For detail you can check /lib/libpfm4/lib/events/* for each pmu events.

"""
import sys
import os

EVENT_FILE_EXTENSION = ".hh"
EVENT_FILE_PATH = "../core/events/"

HEADERS = "#pragma once\n#include <cstdint>\n"
NAMESPACE_BEGIN = "namespace optkit::{}::{}{{\n\t" # added pmu_name to the namespace to define scope for enum
EVENT_CLASS_BEGIN = "enum {} : uint64_t {{\n\t\t"  # didn't create enum class for implicit conversion sake.
PMU_EVENT = "{} = {}, // {}"                       # same functionality preserved.
EVENT_CLASS_END = "\n\t};"
NAMESPACE_END = "\n};"
NAMESPACE_ALIASING = "\n\nnamespace {} = optkit::{}::{};"



PRIV_DEFINITIONS = {
    "intel":[
        "#include \"intel_priv.hh\"\n",
    ]
}

def process_event_dic(event_dict,event_pe): 
    vendor_name = event_pe.split("_")[0]
    pmu_name = event_pe.replace(vendor_name + "_" ,"").replace("_pe","")
    
    if "power" in event_pe:
        vendor_name = "ibm"
        pmu_name = event_pe.replace("_pe","")
    
    result = HEADERS 
    
    for key in PRIV_DEFINITIONS.keys():
        if vendor_name in key:
            for _def in PRIV_DEFINITIONS[key]:
                result = result + _def
    
    result = result + NAMESPACE_BEGIN.format(vendor_name,pmu_name) + EVENT_CLASS_BEGIN.format(pmu_name)
    
    event_counter = {}    
    for event in event_dict[event_pe]: 
        members = event.replace("{","").replace("}","").split(",")
        event_name = next((item for item in members if "name" in item.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
        if len(event_name) > 0: 
            event_name = event_name.split()[0]
            if event_name[0].isdigit():
                event_name = "_"+event_name 
        
        if vendor_name != "ibm":
            event_code = next((item for item in members if ".code" in item.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
            event_desc = next((item for item in members if ".desc" in item.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
        else:
            event_code = next((item for item in members if "code" in item.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
            event_desc = next((item for item in members if "desc" in item.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
            
        event_umasks = next((item for item in members if ".umasks" in item.replace(" "," ")),"").split("=")[-1].replace("\"","").strip()
        if len(event_umasks) > 0:
            event_umasks = event_umasks.split()[0]
        
        if event_counter.get(event_name,0) == 0: ## first encounter to that event_name
            event_counter[event_name] = 1
            result += PMU_EVENT.format(event_name,event_code,event_desc) + "\n\t\t"
        else:
            encounter = event_counter[event_name]
            event_counter[event_name] += 1
            result += PMU_EVENT.format(event_name+"__REPEAT__"+str(encounter),event_code,event_desc) + "\n\t\t"
            
        if len(event_umasks) > 0:
            for mask in event_dict[event_umasks]:
                mask_members = mask.split(",")    
                mask_name = next((mask_val for mask_val in mask_members if ".uname" in  mask_val.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
                mask_code = next((mask_val for mask_val in mask_members if ".ucode" in mask_val.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
                mask_desc = next((mask_val for mask_val in mask_members if ".udesc" in mask_val.replace(" ",""))," ").split("=")[-1].replace("\"","").strip()
                temp_mask = event_name + "__MASK__" + event_umasks.upper() + "__" + mask_name 

                encounter = event_counter.get(temp_mask,0)
                if encounter == 0:
                    event_counter[temp_mask] = 1
                    result += PMU_EVENT.format(temp_mask,mask_code,mask_desc) + "\n\t\t"
                else:
                    event_counter[temp_mask] += 1
                    result += PMU_EVENT.format(temp_mask+"__REPEAT__"+str(encounter),mask_code,mask_desc) + "\n\t\t"

 
    return result + EVENT_CLASS_END + NAMESPACE_END + NAMESPACE_ALIASING.format(pmu_name, vendor_name, pmu_name)

if __name__ == "__main__": 

    if os.path.exists(EVENT_FILE_PATH+"all_set"):
        os.remove(EVENT_FILE_PATH+"all_set")

    if os.path.exists(EVENT_FILE_PATH+"errors"):
        os.remove(EVENT_FILE_PATH+"errors")

    event_dict = {}
    # Get command-line arguments
    event_files = sys.argv[1:]  # The first item in sys.argv is the script's name, so we start from the second item.

    if len(event_files) == 0:
        print("[ERROR]: Enter valid event files")
        sys.exit(1)
         
    for event_file in event_files:
        if not os.path.isfile(event_file):
            if os.path.exists(EVENT_FILE_PATH+"all_set"):
                os.remove(EVENT_FILE_PATH+"all_set")

            err_msg = f"[ERROR]: The file '{event_file}' does not exists!\n"
            print(err_msg)
            with open(EVENT_FILE_PATH+"errors","a") as error_file:
                error_file.write(err_msg)

        else:  
            try: 
                with open(event_file, 'r') as file:
                    lines = file.readlines()
                
                event_lines = ""
                event_pe= ""
                read_event_lines = False
                
                for line in lines:
                    line = line.strip()
                    if line.find("static") != -1:
                        if line.replace(" ","").find("[]") == -1:
                            continue
                        word = [word for word in line.split() if word.find("[") != -1][0]
                        word = word[: word.find("[")]
                        if word.endswith("_pe"):
                            event_pe = word
                        event_dict[word] = []
                        read_event_lines = True
                    elif read_event_lines:
                        if line.endswith("},"):
                            event_lines += line
                            event_dict[word].append(event_lines.strip())
                            event_lines = ""
                        elif line.find("};") == -1:
                            event_lines += line
                        else:
                            read_event_lines = False
                            
                if "power" in event_pe:
                    for _ in event_dict.values():
                        for event_index, event in enumerate(_):
                            _[event_index] = event[event.find("=") + 1:]

                        
                ## do c++ class-enum_class conversion here based on the format
                vendor_name = event_pe.split("_")[0]
                vendor_folder = EVENT_FILE_PATH+vendor_name
                
                if event_pe.count("_") == 1:
                    pmu_name = vendor_name.replace("_pe","")
                else:
                    pmu_name = event_pe.replace(vendor_name + "_" ,"").replace("_pe","")
                 
                
                if not os.path.exists(vendor_folder):
                    os.mkdir(vendor_folder)
                with open(vendor_folder+"/" + pmu_name + EVENT_FILE_EXTENSION, "w") as cpp_file:
                    class_higherachy = process_event_dic(event_dict,event_pe)
                    cpp_file.writelines(class_higherachy)

            except Exception as e:
                print("processing :",event_file,"[ERROR]")
                if os.path.exists(EVENT_FILE_PATH+"all_set"):
                    os.remove(EVENT_FILE_PATH+"all_set")
                with open(EVENT_FILE_PATH+"errors", "a") as error_file:
                    error_file.write(str(e)+"\n")
                continue
            
            print("processing :",event_file,"[OK]")
            
            
            
    if not os.path.exists(EVENT_FILE_PATH+"errors"):
        with open(EVENT_FILE_PATH+"all_set", "a") as f:
            f.write("All SET!\n")

