import ROOT
import numpy as np
import sys, os
import argparse

#
# useful function here:
#

G2FWHM = 2.3548200450309493
def fitSimpleGauss(histo, nsigma=1.5, verbose=True):
    g0 = ROOT.TF1("g0", "gaus", 500, 10000)
    histo.Fit("g0", "RNQ")
    g = ROOT.TF1("g", "gaus", \
                 g0.GetParameter(1) - nsigma*g0.GetParameter(2),\
                 g0.GetParameter(1) + nsigma*g0.GetParameter(2))
    histo.Fit("g", "RQ")
    gPeak  = g.GetParameter(1)
    gSigma = g.GetParameter(2)
    gFWHM  = G2FWHM*(gSigma/gPeak)
    gPeakErr = g.GetParError(1)
    gFWHMErr = gFWHM*np.sqrt((g.GetParError(2)/gSigma)**2 + (gPeakErr/gPeak)**2)
    
    if verbose:
        print  "fitSimpleGauss:", gPeak, gSigma, "Eres=", gSigma/gPeak, \
            "FWHM", gFWHM
    return (histo, gPeak, gFWHM, gPeakErr, gFWHMErr)
    
# 
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
parser.add_argument('-o', '--saveout', action='store_true',
                    help='flag to save final data in a txt file')
parser.add_argument('-t', '--timebin', type=float, default=1.0,
                        help='time interval in hours ')
args = parser.parse_args()

# user selectable options here
Label    = args.label
TimeBin  = args.timebin #hours
SavePlot = args.saveplot
SaveOutput = args.saveout
nsigfit  = 1.5
CUT      = '(%s)' %args.cut
minEvtInHist = 500


# set root style
ROOT.gStyle.SetOptFit(1)
ROOT.gStyle.SetPadGridX(True)
ROOT.gStyle.SetPadGridY(True)



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
Nbins      = max(1, int((maxTime-minTime)/TimeBin))
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

###g0 = ROOT.TF1("g0", "gaus", 3000, 5500)
for i in xrange(Nbins):
    # get projection in time (y coord in hAll)
    htmp = hAll.ProjectionX("htmp", i+1,i+1)
    htmp.Draw()
    if htmp.GetEntries() >= minEvtInHist:
        #print '---> ', htmpnp.GetEntries(), htmp.GetEntries(),\
        #    htmp.GetEntries()/htmpnp.GetEntries()

        (htmp, gPeak, gFWHM, gPeakErr, gFWHMErr) = \
        fitSimpleGauss (htmp,  nsigfit)
        
        timeX.append(timeVal[i])
        timeXErr.append(timeErr[i])
        gainVal.append(gPeak)
        gainErr.append(gPeakErr)
        resVal.append(gFWHM)
        resErr.append(gFWHMErr)

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
gRes.SetTitle("Resolution (gaussian sigma FWHM) @ 5.9 keV (%s)" % Label)
gRes.GetXaxis().SetTitle("Elapsed Time (hours)")
gRes.GetYaxis().SetTitle("#Delta E/E (%)")
gRes.Draw("ap")
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

# Save Gain and Resolution Data in a txt file
if SaveOutput:
    import time
    outFileName = "gaintrend_%s.data" %Label
    outFile = file(outFileName, "w+")
    outFile.write("# Gain and resolution for %s \n" %Label)
    outFile.write("# Written on %s\n" % time.ctime())
    outFile.write("# Time offset (hr) = %f\n" % TimeOffset)
    outFile.write("# time (hr)\tpeak\tfwhm\ttimeErr\tpeakErr\tfwhmErr\n")
    for i in xrange(N):
        outFile.write("%g\t%g\t%g\t%g\t%g\t%g\n" %\
                      (timeX[i], gainVal[i], resVal[i],
                       timeXErr[i], gainErr[i], resErr[i]))
    
    outFile.close()
