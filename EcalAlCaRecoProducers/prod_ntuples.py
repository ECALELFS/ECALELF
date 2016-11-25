#!/usr/local/bin/python
# this a submission script fro crab3

import numpy  as np
import pandas as pd
import sys, os, csv, re, json, getopt
from   StringIO import *
from   pprint   import pprint
from optparse   import OptionParser


def dataset_option(infile,
                   grep = ["RUN2016D","276315-276811"],
                   header=["run_range","dataset","eos","scheduler", "type", "validity", "ERA"]):
    _lines_  = [line for line in open(infile) if not line.startswith('#')]
    _lines_  = [line.replace('\n','') for line in _lines_]
    for g in grep:
        _lines_   = [line for line in _lines_ if g in line ]
    option = [li for li in _lines_[0].split() if li != '']
    pprint(option)
    pprint(header)
    assert not(len(_lines_) != 1)         , "mutiple or no dataset selected : %i" % len (_lines_)
    assert not(len(option) != len(header)), "header and options lentgh are different : (%i,%i)" % (len (option), len(header))
    data_ = dict(zip(header, option))
    data_['remote_dir'] = " ".join([data_['type'], data_['validity'], data_['ERA']])
    print " ------------------------------"
    pprint (data_)
    print " ------------------------------"
    return data_

def json_select(JSON, run_range = "281693-282034", outdir=''):
    range_ = [ int(run) for run in run_range.split("-")]
    with open(JSON, 'r') as f:
        idata = json.load(f)
        odata = {}
        for run, item in idata.items():
            if int(run) >= range_[0] and int(run) <= range_[1]:
                print run
                odata[run] = item
    outname = os.path.join(outdir, 'CRAB3-%s-JSON.txt' % run_range )
    with open(outname, 'w') as f:
        json.dump(odata, f, indent=4)
    return outname

def parse_crab_cfg(options, name):
    crab_template = "python/crab_TEMPLATE.py"
    pprint(options)
    new_cfg = os.path.join(options['submission_dir'], os.path.basename( crab_template.replace('TEMPLATE', name)) )
    with open(crab_template, "r") as inf, open(new_cfg, "w") as outf:
        for line in inf:
            line = line.replace('{DATASET}' , options['dataset'] )
            if "{DATASET}"  in line : line = line.replace('{DATASET}' , options['dataset'   ] )
            if "{TYPE}"     in line : line = line.replace('{TYPE}'    , options['type'      ] )
            if "{GT}"       in line : line = line.replace('{GT}'      , options['GT'        ] )
            if "{JSONFILE}" in line : line = line.replace('{JSONFILE}', options['json_file' ] )
            if "{RUNRANGE}" in line : line = line.replace('{RUNRANGE}', options['run_range' ] )
            if "{EOSDIR}"   in line : line = line.replace('{EOSDIR}'  , options['remote_dir'] )
            if "{RUNONCAF}" in line : line = line.replace('{RUNONCAF}', options['run_on_caf'] )
            outf.write(line)
    print " ------------------------------"
    print " --- crab file :: ", crab_template.replace('TEMPLATE', name)
    os.system('cat ' + new_cfg )
    print " ------------------------------"

def get_options():
    parser = OptionParser()
    parser.add_option('-j','--json-file',
                      dest='json_file',default='RUN2016H-281613-282037_13TeV_PromptReco_JSON_NoL1T.txt',
                      help="""specify the JSON file""", metavar="FILE")

    parser.add_option("-r", "--run-range",
                      dest="run_range", default='281693-282034',
                      help="""
                      Specify the run range. This must be present in the input
                      JSON file, and in the dataset file. The range shoud be
                      specified for example as :
                      --run-range "281613-282037"
                      """)
    parser.add_option("-n", "--name",
                      dest="name", default='crab3-job-miniAOD',
                      help="""
                      name given to the job
                      """)
    parser.add_option('-q', '--run-on-caf',
                      dest='run_on_caf',default=False,
                      help="""
                      Option allowing CRAB jobs to run on CAF queues:
                      If you want to choose a specific site you can edit
                      the generated crab config file
                      """)
    parser.add_option('-d','--dataset-file',
                      dest='dataset_file',default='alcareco_datasets.dat',
                      help="""
                      dataset file containing the information about the run range:
                      example :
                      | run-range | dataset path | store path radical in eos | storing unit | ......
                      281613-282037	/DoubleEG/Run2016H-PromptReco-v2/MINIAOD DoubleEG-Run2016H-noSkim-Prompt_v2-miniAOD caf database VALID RUN2016H
                      """, metavar="FILE")
    parser.add_option('-g','--global-tag',
                      dest='global_tag',default='config/reRecoTags/80X_dataRun2_Prompt_v12.py',
                      help="""
                      Global Tag to the corresponding dataset :
                      example :
                      config/reRecoTags/80X_dataRun2_Prompt_v12.py
                      """, metavar="FILE")
    parser.add_option("-t", "--type",
                      dest="type", default='MINIAODNTUPLE',
                      help="""
                      Type of the ntuple : MINIAODNTUPLE
                      """)
    parser.add_option('--create-only',
                      dest="create_only", default=True,
                      help="""This will crate JSON file and crab3 config file without submission""")
    parser.add_option('--submit',
                      dest="submit", default=True,
                      help="""This will crate JSON file and crab3 config files and proceseed to submission:
                      This command is equivalent of running : crab python/
                      """)
    return parser.parse_args()

if __name__ == '__main__':
    (opt, args) =  get_options()

    print " ------------------------------"
    print " run-range  : ", opt.run_range
    print " json-file  : ", opt.json_file
    print " run on CAF : ", opt.run_on_caf
    print " global tag : ", opt.global_tag
    print " ------------------------------"
    op = dataset_option("alcareco_datasets.dat",
                        grep = [opt.run_range, opt.name],)
    new_json = json_select(opt.json_file, opt.run_range)
    op['GT'        ] = opt.global_tag
    op['json_file' ] = new_json
    op['type'      ] = opt.type
    op['run_on_caf'] = opt.run_on_caf

    options="-d ${DATASETPATH} -n ${DATASETNAME} -r ${RUNRANGE} --remote_dir ${ORIGIN_REMOTE_DIR_BASE}"
    origin_remote_dir_base = "/store/group/dpg_ecal/alca_ecalcalib/ecalelf/ntuples/13TeV/"
    remote_dir_base = ''
    if "ALCARERECO" in op['type']:
        print "running on ", op['type']
        print "not implemented for now"
    if "MINIAODNTUPLE" in op['type']:
        print " ------------------------------"
        op['remote_dir'] = '/'.join([
            origin_remote_dir_base,
            op['type'     ],
            os.path.splitext(os.path.basename(op['GT']))[0],
            op['eos'      ],
            op['run_range']]
        ) + '/'
        print op['remote_dir']
        print " ------------------------------"
    op['submission_dir'] = 'crab-submit-%s-%s' % (op['ERA'], opt.run_range)
    if not os.path.exists(op['submission_dir']):
        os.makedirs(op['submission_dir'])
    parse_crab_cfg(op , name='crab-%s-%s' % (opt.run_range, op['ERA']))
    print " ------------------------------"
    json_select   (new_json, run_range = opt.run_range, outdir= op['submission_dir'])
    print " ------------------------------"
