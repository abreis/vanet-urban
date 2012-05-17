# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('vanet-urban', ['core', 'wifi', 'mobility', 'tools'])
    module.source = [
        'model/City.cc',
        'model/VanetHeader.cc',
        'model/Vehicle.cc',
        ]

    headers = bld.new_task_gen(features=['ns3header'])
    headers.module = 'vanet-urban'
    headers.source = [
        'model/City.h',
        'model/VanetHeader.h',
        'model/Vehicle.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.add_subdirs('examples')
