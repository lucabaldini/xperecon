import ROOT
import sys, os

# TODO:
# organize a couple of functions,
# add options for saving, cut, step
# loop on areas and plot

import argparse

formatter = argparse.ArgumentDefaultsHelpFormatter
parser = argparse.ArgumentParser(formatter_class=formatter)
parser.add_argument('infile', type=str, nargs='+',
                    help='the input binary file list')
parser.add_argument('-l', '--label', type=str, default='GEM test',
                    help='labels for plots')
#parser.add_argument('-n', '--num-events', type=int, default=10,
#                        help='number of events to be read')
#parser.add_argument('-z', '--zero-suppression', type=int, default=10,
#                    help='zero-suppression threshold')
args = parser.parse_args()

Label = args.label

t = ROOT.TChain('tree')
for f in args.infile:
    if os.path.isfile(f):
        t.Add(f)
    else:
        print ('Skipping non existing file %s' %f)
        
NTotEvt = t.GetEntries()
if NTotEvt>1000:
    print('Number of events %d' % NTotEvt)
else:
    print('Not enougth events %d, exiting' % NTotEvt)
    sys.exit(1)

#CUT = "abs(fBaricenterY[0]-5)<1 && abs(fBaricenterX[0]+5)<1"
CUT = "abs(fBaricenterY[0]-4)<1 && abs(fBaricenterX[0]+4)<1"
#CUT = "(1)"

c = ROOT.TCanvas("plot_%s"%Label, "plot_%s"%Label, 800, 600)
c.Divide(2,1)
c.cd(1)
t.Draw("fBaricenterY[0]:fBaricenterX[0]>>hh(160,-8,8,160,-8,8)", CUT, "colz")

myTxt = ROOT.TLatex()
myTxt.SetTextFont(42)

c.cd(2)
hPHeight = ROOT.TH1F("hPHeight", "hPHeight", 1000,0, 10000)
t.Draw("fPHeight[0]>>hPHeight", CUT)

print("\n FIT MAIN PEAK")
peak = hPHeight.GetBinCenter(hPHeight.GetMaximumBin())
rms  = hPHeight.GetRMS()
g0 = ROOT.TF1("g0", "gaus", peak-rms, peak+rms)
hPHeight.Fit("g0", "R")
myTxt.DrawLatexNDC(0.6, 0.75, "Main Peak:")
myTxt.DrawLatexNDC(0.6, 0.7, "Ave %d" %(g0.GetParameter(1)))
myTxt.DrawLatexNDC(0.6, 0.65, "FWHM %.2f%%" %(235.*g0.GetParameter(2)/g0.GetParameter(1)))
g0.Draw("same")

print("\n FIT ESC. PEAK")
esc  = g0.GetParameter(1)/2
rms1 = g0.GetParameter(2)
g1 = ROOT.TF1("g1", "gaus", esc-rms1, esc+rms1)
g1.SetLineColor(3)
hPHeight.Fit("g1", "R")
g1.Draw("same")
myTxt.DrawLatexNDC(0.6, 0.6, "Esc Peak:")
myTxt.DrawLatexNDC(0.6, 0.55, "Ave %d" %(g1.GetParameter(1)))
myTxt.DrawLatexNDC(0.6, 0.5, "FWHM %.2f%%" %(235.*g1.GetParameter(2)/g1.GetParameter(1)))

c.Update()







