#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Created on Tue Apr 23 13:19:06 2024

@author: rt7
"""

import json
import os

file_names = []
durations_ms = []
socket0_energy_cores_values = []
socket0_energy_pkg_values = []
socket0_energy_ram_values = []
socket0_energy_gpu_values = []
socket0_energy_psys_values = []
socket0_energy_total = []
socket0_power = []
socket0_edp = [] 

socket1_energy_cores_values = []
socket1_energy_pkg_values = []
socket1_energy_ram_values = []
socket1_energy_gpu_values = []
socket1_energy_psys_values = []
socket1_energy_total = []
socket1_power = []
socket1_edp = [] 


def get_json_files(directory, endswith = ".json", is_sorted=True):
    json_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(endswith):
                json_files.append(os.path.join(root, file))
    for file in sorted(json_files):
        print(file)
    # Sort the list of JSON files
    return json_files if not is_sorted else sorted(json_files)

def write_json_to_file(json_string, filename, verbose = True):
    if verbose:
        print("writing to:",filename)
        
    with open(filename, 'w') as file:
        file.write(json_string)


def load_measurements(json_file):

    # read file
    with open(json_file, "r") as file:
        data = json.load(file)

    file_names.append(json_file)
    duration = data[0]["readings"][0]["duration"]
    durations_ms.append(duration)

    for reading in data[0]["readings"]:
        socket = reading["package_number"]
        for metric in reading["metrics_set"]:
            if metric["metric_name"] == "energy-pkg":
                energy_pkg = metric["value"]
                if socket == 0:
                    socket0_energy_pkg_values.append(energy_pkg)
                else:
                    socket1_energy_pkg_values.append(energy_pkg)

            elif metric["metric_name"] == "energy-ram":
                energy_ram = metric["value"]
                if socket == 0:
                    socket0_energy_ram_values.append(energy_ram)
                else:
                    socket1_energy_ram_values.append(energy_ram)

            elif metric["metric_name"] == "energy-gpu":
                energy_gpu = metric["value"]
                if socket == 0:
                    socket0_energy_gpu_values.append(energy_gpu)
                else:
                    socket1_energy_gpu_values.append(energy_gpu)

            elif metric["metric_name"] == "energy-psys":
                energy_psys = metric["value"]
                if socket == 0:
                    socket0_energy_psys_values.append(energy_psys)
                else:
                    socket1_energy_psys_values.append(energy_psys)

            elif metric["metric_name"] == "energy-total":
                energy_total = metric["value"]
                if socket == 0:
                    socket0_energy_total.append(energy_total)
                else:
                    socket1_energy_total.append(energy_total)

            elif metric["metric_name"] == "power":
                power = metric["value"]
                if socket == 0:
                    socket0_power.append(power)
                else:
                    socket1_power.append(power)

            elif metric["metric_name"] == "edp":
                edp = metric["value"]
                if socket == 0:
                    socket0_edp.append(edp)
                else:
                    socket1_edp.append(edp) 
        
        
        
        