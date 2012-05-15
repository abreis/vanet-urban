## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):    
    vaneturban = bld.create_ns3_program('vaneturban', ['core', 'mobility', 'wifi', 'applications', 'tools'])
    vaneturban.source = [
	'main.cc',
	'City.cc',
	'Vehicle.cc',
#	'Model.cc',
#	'LaneChange.cc',
	'VanetHeader.cc'
	]

    headers = bld.new_task_gen(features=['ns3header'])
    headers.module = 'vaneturban'
    headers.source = [
	'City.h',
	'Vehicle.h',
#	'Model.h',
#	'LaneChange.h',
	'VanetHeader.h'
	]

