import ROOT
import numpy as np
import sys, os
import argparse

formatter = argparse.ArgumentDefaultsHelpFormatter
parser = argparse.ArgumentParser(formatter_class=formatter)
parser.add_argument('infile', type=str, nargs='+',
                    help='the input binary file list')
parser.add_argument('-l', '--label', type=str, default='Gain test',
                    help='labels for plots')
parser.add_argument('-c', '--cut', type=str, default='1',
                    help='ROOT-valid cut string')
parser.add_argument('-s', '--saveplot', action='store_true',
                    help='flag to save final plot')
parser.add_argument('-t', '--timebin', type=float, default=1.0,
                        help='time interval in hours ')
args = parser.parse_args()

# user selectable options here
Label    = args.label
TimeBin  = args.timebin #hours
SavePlot = args.saveplot
nsigfit  = 1.5
CUT      = '(%s)' %args.cut
minEvtInHist = 500
useFWHM      = True

# set root style
ROOT.gStyle.SetOptFit(1)
ROOT.gStyle.SetPadGridX(True)
ROOT.gStyle.SetPadGridY(True)
G2FWHM = 2.3548200450309493


tt = ROOT.TChain('tree')
for f in args.infile:
    if os.path.isfile(f):
        tt.Add(f)
    else:
        print ('Skipping non existing file %s' %f)
        
NTotEvt = tt.GetEntries()
if NTotEvt>1000:
    print('Number of events %d' % NTotEvt)
else:
    print('Not enougth events %d, exiting' % NTotEvt)
    sys.exit(1)

TimeOffset = tt.GetMinimum('fTimeStamp')
print('TimeOffset %f' %TimeOffset)
TimeHours  = "((fTimeStamp-%.1f)/3600.)" % TimeOffset
minTime    = 0 # by definition
maxTime    = (tt.GetMaximum("fTimeStamp")-TimeOffset)/3600.
Nbins      = max(1,int((maxTime-minTime)/TimeBin))
timeBins   = np.linspace(0,maxTime, Nbins+1)
print "Eval trend from %f hr to %f hr after start in %d bins" %\
    (min(timeBins), max(timeBins), len(timeBins))

Nbins    = len(timeBins)-1
timeVal  = 0.5*(timeBins[1:] +timeBins[:-1])
timeErr  = 0.5*(timeBins[1:] -timeBins[:-1])
timeX    = []
timeXErr = []
gainVal  = []
gainErr  = []
resVal   = []
resErr   = []
chbins   = np.linspace(0,10000,201)

# project entire tree in a 2D histo
hAll = ROOT.TH2F("hAll", "hAll_%s"% Label, len(chbins)-1, chbins, \
                 len(timeBins)-1, timeBins)
tt.Project("hAll", "%s:fPHeight[0]" % TimeHours, CUT)
if hAll.GetEntries()<minEvtInHist:
    print "Only %d events in histogram: exiting" % hAll.GetEntries()
    sys.exit(1)
    

ctmp = ROOT.TCanvas("cGainTend_%s" % Label, "cGainTend_%s" % Label)
allHistoFileName = "GainTrendAllHisto_%s.ps" % Label
ctmp.Print(allHistoFileName +"[");
hAll.Draw("colz")
ctmp.Print(allHistoFileName);

g0 = ROOT.TF1("g0", "gaus", 3000, 5500)
for i in xrange(Nbins):
    # get projection in time (y coord in hAll)
    htmp = hAll.ProjectionX("htmp", i+1,i+1)
    htmp.Draw()
    if htmp.GetEntries() >= minEvtInHist:
        #print '---> ', htmpnp.GetEntries(), htmp.GetEntries(),\
        #    htmp.GetEntries()/htmpnp.GetEntries()
        
        htmp.Fit("g0", "RNQ")
        g = ROOT.TF1("g", "gaus", \
                     g0.GetParameter(1) - nsigfit*g0.GetParameter(2),\
                     g0.GetParameter(1) + nsigfit*g0.GetParameter(2))
        htmp.Fit("g", "RQ")
        timeX.append(timeVal[i])
        timeXErr.append(timeErr[i])
        gPeak  = g.GetParameter(1)
        gSigma = g.GetParameter(2)
        print  "\n",i, gPeak, gSigma, "Eres=", gSigma/gPeak, \
            "FWHM", 2.355*(gSigma/gPeak)
        gainVal.append(gPeak)
        gainErr.append(g.GetParError(1))
        resVal.append(100.*gSigma/gPeak)
        resErr.append(resVal[-1]*(g.GetParError(2)/gSigma )) # ~ok for now
        if useFWHM:
            resVal[-1] = resVal[-1]*G2FWHM
            resErr[-1] = resErr[-1]*G2FWHM
    ROOT.gPad.Update()
    ctmp.Print(allHistoFileName);

ctmp.Print(allHistoFileName +"]");


# plot final result

timeX    = np.array(timeX)
timeXErr = np.array(timeXErr)
gainVal  = np.array(gainVal)
gainErr  = np.array(gainErr)
resVal   = np.array(resVal)
resErr   = np.array(resErr)
N        = len(timeX)

cTrend = ROOT.TCanvas("gaintrend_%s" %Label,\
                      "gaintrend_%s" %Label, 1000, 700)
cTrend.Divide(1,2)
cTrend.cd(1)
gGain = ROOT.TGraphErrors(N, timeX*3600, gainVal, timeXErr*3600, gainErr)
gGain.SetMarkerStyle(20)
gGain.SetTitle("Gain (gaussian peak) @ 5.9 keV (%s)" % Label)
gGain.GetYaxis().SetTitle("Peak")
gGain.GetXaxis().SetTitle("")
gGain.GetXaxis().SetTimeDisplay(1);
gGain.GetXaxis().SetTimeFormat("#splitline{%d/%m/%y}{%H:%M:%S}");
gGain.GetXaxis().SetTimeOffset(TimeOffset)
gGain.GetXaxis().SetLabelOffset(0.03)
gGain.Draw("ap")
#fGain = ROOT.TF1('fGain', 'pol1', 0, (SpotCut_tthr-TimeOffset))
#fGain.SetParameters(10, 0.002)
#gGain.Fit("fGain", "R")
ROOT.gPad.Update()
try:
    psGain = gGain.GetListOfFunctions().FindObject("stats");
    psGain.SetX1NDC(0.65);
    psGain.SetY1NDC(0.18);
    psGain.SetX2NDC(0.87);
    psGain.SetY2NDC(0.34);
except:
    pass
cTrend.cd(2)
gRes = ROOT.TGraphErrors(N, timeX, resVal, timeXErr, resErr)
gRes.SetMarkerStyle(20)
gRes.SetTitle("Resolution (gaussian sigma/peak) @ 5.9 keV (%s)" % Label)
if useFWHM:
    gRes.SetTitle("Resolution (gaussian sigma FWHM) @ 5.9 keV (%s)" % Label)
gRes.GetXaxis().SetTitle("Elapsed Time (hours)")
gRes.GetYaxis().SetTitle("#Delta E/E (%)")
gRes.Draw("ap")
#fRes = ROOT.TF1('fRes', 'pol1', 0, (SpotCut_tthr-TimeOffset)/3600.)
#fRes.SetParameters(0.1, 0.001)
#gRes.Fit("fRes", "R")
ROOT.gPad.Update()
try:
    psRes = gRes.GetListOfFunctions().FindObject("stats");
    psRes.SetX1NDC(0.33);
    psRes.SetY1NDC(0.64);
    psRes.SetX2NDC(0.54);
    psRes.SetY2NDC(0.80);
except:
    pass
cTrend.Update()

if SavePlot:
    cTrend.SaveAs("%s.png" % cTrend.GetName())
