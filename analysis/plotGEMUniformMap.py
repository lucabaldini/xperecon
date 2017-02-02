import ROOT
import sys, os
import numpy

# TODO:
# organize a couple of functions,
# add options for saving, cut, step
# loop on areas and plot

import argparse

formatter = argparse.ArgumentDefaultsHelpFormatter
parser = argparse.ArgumentParser(formatter_class=formatter)

parser.add_argument('-infiles', '--infiles', type=str, nargs='+',
                    default=None,
                    help='the input root file list')
parser.add_argument('-l', '--label', type=str, default='GEM uniformities',
                    help='labels for plots')
parser.add_argument('-out', '--out', type=str,
                    default='gem_uniformities_scan.txt',
                    help='Name of output file with the scan information')
parser.add_argument('-n', '--num_events', type=int, default=1e5,
                        help='number of events to be read')
#parser.add_argument('-z', '--zero-suppression', type=int, default=10,
#                    help='zero-suppression threshold')


args = parser.parse_args()

label = args.label
input_file_path_list = args.infiles
output_file_name = args.out

#These are the bounds over which the scan will be performed, they are in
#reference to the baricenter X and Y values.

min_bound = -8
max_bound = 8
num_points = 19



def runScan():
    """
    This method performs a scan over the Baricenter map and fits the data
    to obtain the fwhm and position of the main peak, as well as the
    fwhm and postion of the escape peak. These values are then saved to
    an output txt file to be read in by the method plotGEMUniformities.

    """
    t = ROOT.TChain('tree')
    for f in input_file_path_list:
        if os.path.isfile(f):
            t.Add(f)
        else:
            print ('Skipping non existing file %s' %f)
        
    NTotEvt = t.GetEntries()
    if NTotEvt> args.num_events :
        print('Number of events %d' % NTotEvt)
    else:
        print('Not enougth events %d, exiting' % NTotEvt)
        sys.exit(1)

    output_file = file(output_file_name,'w')
    line = "#x,y,num_events,rms,peak,rms1\n"

    x_bins = numpy.linspace(min_bound, max_bound, num_points)
    y_bins = numpy.linspace(min_bound, max_bound, num_points)

    
    for i,x in enumerate(x_bins):
        xcut = "abs(fBaricenterX[0]+%s)<1"%x
        for j,y in enumerate(y_bins):
            ycut = "abs(fBaricenterY[0]-%s)<1"%y
            cut = "%s && %s"%(ycut,xcut)
            
            print "Using the following cut ",cut
         
            c = ROOT.TCanvas("plot_%s"%label, "plot_%s"%label, 800, 600)
            c.Divide(2,1)
            c.cd(1)
            t.Draw("fBaricenterY[0]:fBaricenterX[0]>>hh(160,-8,8,160,-8,8)", cut, "colz")
            num_events = t.GetEntries(cut)
            myTxt = ROOT.TLatex()
            myTxt.SetTextFont(42)

            c.cd(2)
            hPHeight = ROOT.TH1F("hPHeight", "hPHeight", 1000,0, 10000)
            t.Draw("fPHeight[0]>>hPHeight", cut)

            print("\n FIT MAIN PEAK")
            peak = hPHeight.GetBinCenter(hPHeight.GetMaximumBin())
            rms  = hPHeight.GetRMS()
            g0 = ROOT.TF1("g0", "gaus", peak-rms, peak+rms)
            hPHeight.Fit("g0", "R")
            myTxt.DrawLatexNDC(0.6, 0.75, "Main Peak:")
            myTxt.DrawLatexNDC(0.6, 0.7, "Ave %d" %(g0.GetParameter(1)))
            try:
                fwhm = 235.*g0.GetParameter(2)/g0.GetParameter(1)
                myTxt.DrawLatexNDC(0.6, 0.65, "FWHM %.2f%%" %(fwhm))
            except:
                print "ZeroDivision Error"
                fwhm = 0.0
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
            try:
                fwhm1 = 235.*g1.GetParameter(2)/g1.GetParameter(1)
                myTxt.DrawLatexNDC(0.6, 0.5, "FWHM %.2f%%" %(fwhm1))
            except:
                print "ZeroDivision Error"
                fwhm1 = 0.0
            line += "%s,%s,%s,%s,%s,%s\n"%(x,y,num_events,fwhm,peak,fwhm1)
            #c.Update()
            
    output_file.write(line)
        
    output_file.close()
    

def plotGEMUniformities():
    """
    Simple method to read in the txt file with the values writen in the
    output txt file produced by the runScan method and generates the
    maps of the fwhm and position of the main peak as well as the 
    distribution of the number of events over the scaned area.
    """
    
    from matplotlib import pyplot as plt
    import matplotlib
    matplotlib.rc('figure', facecolor='white') 
  
    fig = plt.figure()
 
    x,y,num_events,fwhm,peak,fwhm1 = numpy.loadtxt(output_file_name, delimiter=',', unpack=True)
     
    x_bins = numpy.linspace(min_bound, max_bound, num_points)
    y_bins = numpy.linspace(min_bound, max_bound, num_points)

    x_bins, y_bins = numpy.meshgrid(x_bins, y_bins)
    

    fwhm_array = fwhm.reshape((num_points,num_points))  
    ax1 = fig.add_subplot()
    label = '%s'%(args.label)

    plt.contourf(x_bins, y_bins,fwhm_array)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('FWHM of main peak')
    plt.text(0.02, 0.92, label, transform=plt.gca().transAxes,
             fontsize=20,color='w')
    plt.xlabel('fBaricenterX[0]')
    plt.ylabel('fBaricenterY[0]')

    plt.show()

    num_events_array = num_events.reshape((num_points,num_points)) 
    plt.contourf(x_bins, y_bins,num_events_array)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('Number of Events')
    plt.text(0.02, 0.92, label, transform=plt.gca().transAxes,
             fontsize=20,color='w')
    plt.xlabel('fBaricenterX[0]')
    plt.ylabel('fBaricenterY[0]')
    plt.show()

    peak_array = peak.reshape((num_points,num_points))
    plt.contourf(x_bins, y_bins,peak_array)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('Peak')
    plt.text(0.02, 0.92, label, transform=plt.gca().transAxes,
             fontsize=20,color='w')
    plt.xlabel('fBaricenterX[0]')
    plt.ylabel('fBaricenterY[0]')
    plt.show()
    """
    fwhm1_array = fwhm1.reshape((num_points,num_points))  
    ax1 = fig.add_subplot()
    label = '%s'%(label)

    plt.contourf(x_bins, y_bins,fwhm1_array)
    cbar = plt.colorbar()
    cbar.ax.set_ylabel('FWHM of escape peak')
    plt.text(0.02, 0.92, label, transform=plt.gca().transAxes,
             fontsize=20,color='w')
    plt.xlabel('fBaricenterX[0]')
    plt.ylabel('fBaricenterY[0]')

    plt.show()
    """


if input_file_path_list is not None:
    output_file_name = runScan()
   
plotGEMUniformities()
