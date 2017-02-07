import ROOT
import sys
import os
import numpy
import pha



def run_analysis(output_file_path,
                 cut = '', num_events = 100000000, label='GEM', *args):

    chain = ROOT.TChain("tree")
    for file_path in args:
        print file_path
        chain.Add(file_path)

    expr='fPHeight[0]:fBaricenterY[0]:fBaricenterX[0]'
    cube = pha.ixpePulseHeightCube('ccube', 'Count cube',label=label)
    chain.Project('ccube', expr, cut, '', num_events)
    cube.project_pha()
    
    if output_file_path is not None:
        cube.write(output_file_path)

    if sys.flags.interactive:
        cube.draw('colz')
    return cube


if __name__ == '__main__':

    import argparse
    
    formatter = argparse.ArgumentDefaultsHelpFormatter
    parser = argparse.ArgumentParser(formatter_class=formatter)

    parser.add_argument('infiles', type=str, nargs='+',
                        help='the input root file list')
    parser.add_argument('-l', '--label', type=str, default='GEM uniformities',
                        help='labels for plots')
    parser.add_argument('-o', '--out', type=str,
                        default='gem_uniformities.root',
                        help='Name of output file with the scan information')
    parser.add_argument('-n', '--num_events', type=int, default=1000000000,
                        help='number of events to be read')
    parser.add_argument('-c', '--cut', type=str, default='1',
                        help='ROOT-valid cut string')


    args = parser.parse_args()

    
    cube = run_analysis(args.out, args.cut, args.num_events, args.label, *args.infiles)

    
