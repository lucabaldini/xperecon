from  gem_run_dict import GEM_RUN_DICT as dictionary

import os
import ROOT
import __root__
import numpy


def parse_and_fit(gem, useExpression=False):
    file_str = ''
    #    for run in [928,929,930]:#dictionary[gem]['unif_illumination_runs']:
    for run in [925,926,927]:#dictionary[gem]['unif_illumination_runs']:
        filepath = '../out/001_0000%s_data_TH5_outputfile.root'%run
        #gain_trend_file_path = '/data/xpe/xperecon/analysis/gemNov2016Summary/gaintrend_%s.data'%gem
        gain_trend_file_path = '/data/xpe/xperecon/analysis/gaintrend_%s.data'%gem
        file_str+='%s '%filepath
       
    output_file_path = '%s_uniformities.root'%gem
    if useExpression:
        expression = fit_gain_trend(gain_trend_file_path,gem)
        print "Using this expression ",expression
    cmd = 'python plotGEMUniformMap.py %s -o %s -l %s -x "%s"'%(file_str, output_file_path, gem, expression)
    os.system(cmd)

def make_maps(run, label):

    _label = '%s_run%s'%(label,run)
    output_file_path = '/data/xpe/xperecon/analysis/%s_uniformities.root'%(_label)
    cmd = 'python plotGEMUniformMap.py %s -o %s -l %s '%(file_path, output_file_path, _label)
    os.system(cmd)
    parse_and_plot2d(_label)


def run_trending(file_path_list,cut,label,tbining=None):
    file_str = ""
    for file_path in file_path_list:
        file_str+=" %s"%file_path
    output_file_path = 'gaintrend_%s.data'%label
    if os.path.exists(output_file_path):
        return output_file_path

    else:
        if tbining is not None:
            cmd = 'python plotGainTrend.py %s -l %s -t %s -c "%s" -o -s'%(file_str, label, tbining, cut)
        else:
            cmd = 'python plotGainTrend.py %s -l %s -c "%s" -o -s'%(file_str, label, cut)
        
        os.system(cmd)
    
    return output_file_path
    
def plot_trending_by_zone(g_file_path,b_file_path,vg_file_path,center_file_path):
    time, g_peak, g_fwhm, g_timeErr, g_peakErr, g_fwhmErr = \
                        numpy.loadtxt(g_file_path, delimiter='\t', unpack=True)

    time, b_peak, b_fwhm, b_timeErr, b_peakErr, b_fwhmErr = \
                        numpy.loadtxt(b_file_path, delimiter='\t', unpack=True)

    time, vg_peak, vg_fwhm, vg_timeErr, vg_peakErr, vg_fwhmErr = \
                numpy.loadtxt(vg_file_path, delimiter='\t', unpack=True)

    time, c_peak, c_fwhm, c_timeErr, c_peakErr, c_fwhmErr = \
                    numpy.loadtxt(center_file_path, delimiter='\t', unpack=True)

    
    from matplotlib import pyplot as plt
    import matplotlib
    matplotlib.rcParams.update({'font.size': 18})
    fig = plt.figure(figsize=(15,10),facecolor='w')
    tstart = 20
    
    plt.errorbar(time,g_fwhm, xerr=0, yerr=g_fwhmErr, label='Good region', marker='o',ls='none')
    g_pol_coef = numpy.polyfit(time[time>tstart], g_fwhm[time>tstart], deg=1)
    plt.plot(time[time>tstart], numpy.polyval(g_pol_coef, time[time>tstart]),lw=2,color='b')
    plt.text(32, 0.35, 'y = %sx + %s'%(g_pol_coef[0],g_pol_coef[1]),fontsize=18, color='b')
    
    plt.errorbar(time,b_fwhm, xerr=0, yerr=b_fwhmErr, label='Bad region',color='r', marker='o',ls='none')
    b_pol_coef = numpy.polyfit(time[time>tstart], b_fwhm[time>tstart], deg=1)
    plt.plot(time[time>tstart], numpy.polyval(b_pol_coef, time[time>tstart]),lw=2,color='r')
    plt.text(32, 0.32, 'y = %sx + %s'%(b_pol_coef[0],b_pol_coef[1]),fontsize=18, color='red')

    plt.errorbar(time,c_fwhm, xerr=0, yerr=c_fwhmErr, label='Center region',color='black', marker='o',ls='none')
    c_pol_coef = numpy.polyfit(time[time>tstart], c_fwhm[time>tstart], deg=1)
    plt.plot(time[time>tstart], numpy.polyval(c_pol_coef, time[time>tstart]),lw=2,color='black')
    plt.text(32, 0.37, 'y = %sx + %s'%(c_pol_coef[0],c_pol_coef[1]),fontsize=18, color='black')

    plt.errorbar(time,vg_fwhm, xerr=0, yerr=vg_fwhmErr, label='Variable Gain region',color='g', marker='o',ls='none')
    vg_pol_coef = numpy.polyfit(time[time>tstart], vg_fwhm[time>tstart], deg=1)
    plt.plot(time[time>tstart], numpy.polyval(vg_pol_coef, time[time>tstart]),lw=2,color='g')
    plt.text(32, 0.29, 'y = %sx + %s'%(vg_pol_coef[0],vg_pol_coef[1]),fontsize=18, color='green')
    
    plt.xlabel('Time [hr]')
    plt.ylabel('DeltaE/E (%)')
    plt.ylim(0.15,0.40)
    plt.grid()

    plt.legend()
    plt.show()
    

    
def fit_gain_trend(filepath, gem_label):
    time, peak, fwhm, timeErr, peakErr, fwhmErr = \
                        numpy.loadtxt(filepath, delimiter='\t', unpack=True)

    offset = fetch_offset_time(filepath)
    offset = float(offset)
    from matplotlib import pyplot as plt
    print "Ploting peak position for %s"%gem_label
    plt.errorbar(time, peak, xerr=timeErr, yerr=peakErr, marker='o',ls='none')
    pol_deg = 5
    pol_coef = numpy.polyfit(time, peak,deg=pol_deg)

    def time_trend_expr(pol_coef, offset, time_var='fTimeStamp'):
        """
        """
        expr = ''
        degree = len(pol_coef)
        for i, p in enumerate(pol_coef):
            if i < degree - 1:
                expr += '%f*((%s - %f)/3600.)**%d + ' %\
                        (p, time_var, offset, degree - i - 1)
            else:
                expr += '%f' % p
        expr = '(%s)/%f' % (expr, p) 
        return expr

    expr =  time_trend_expr(pol_coef, offset)
    print expr
    
    plt.plot(time, numpy.polyval(pol_coef, time))
    plt.xlabel('Time since start of run [hr]')
    plt.ylabel('Pulse height')
    print "Here are the polynomial coefficients for the fit with pol degree 5",\
        pol_coef
    
    plt.savefig('gain_trend_%s_fit_pol_%s.png'%(gem_label,pol_deg))
    plt.show()

    return expr
    
def fetch_offset_time(gain_trend_file_path):
    gain_file = file(gain_trend_file_path,'r')
    for line in gain_file.readlines():
        if 'offset' in line:
            l = line.split()
            offset_time = l[5]

    return offset_time


def get_stats(hist):
    bin_content_list = []
    for i in range(hist.GetNbinsX()):
        for j in range(hist.GetNbinsY()):
            hist_bin_content = hist.GetBinContent(i+1,j+1)
            bin_content_list.append(hist_bin_content)
            
    bin_content_array = numpy.array(bin_content_list)
    min_val = numpy.amin(bin_content_array)
    max_val = numpy.amax(bin_content_array)
    mean_val = numpy.mean(bin_content_array)
    rms_val = numpy.std(bin_content_array)

    return min_val, max_val,mean_val,rms_val
    
def parse_and_plot2d(label):
    #for gem in dictionary.keys():
    #for gem in ['gem_1420']:
    """This script will make a two panel plot with the energy resolution and main peak position 
    for a given detector. The detector is identified via the label which you pass, the file
    which will be used is the output from plotGEMUniformMap (i.e. root file with the fits for
    each baricenter bin)
    """
    root_file_path = '%s_uniformities.root'%label

    canvas = ROOT.TCanvas("%s"%label,"%s"%label,1200,700)
    canvas.Divide(2,1)
    canvas.cd(1)
    f = ROOT.TFile(root_file_path)
    fwhm_label = "FWHM_%s"%label
    main_peak_label = "MainPeak_%s"%label

    fwhm = f.Get(fwhm_label)
    min_val, max_val,mean_val,rms_val = get_stats(fwhm)
    fwhm_title = "%s FWHM %.3f-%.3f <%.3f> rms:%.3f"%(label,min_val, max_val,mean_val,rms_val)
    fwhm.SetTitle(fwhm_title)
    fwhm.GetYaxis().SetTitle("fBaricenterY[0]")
    fwhm.GetXaxis().SetTitle("fBaricenterX[0]")
    
    fwhm.Draw("colz")
    canvas.Update()
    
    canvas.cd(2)
    peak = f.Get(main_peak_label)
    p_min_val, p_max_val,p_mean_val,p_rms_val = get_stats(peak)
    main_peak_title = "%s MainPeak %.3f-%.3f <%.3f> rms:%.3f"%(label,p_min_val, p_max_val,p_mean_val,p_rms_val)
    peak.SetTitle(main_peak_title)
    peak.GetYaxis().SetTitle("fBaricenterY[0]")
    peak.GetXaxis().SetTitle("fBaricenterX[0]")
    peak.Draw("colz")
    
    canvas.SaveAs("%s_maps.png"%label)
    


def plot_hist(array, gem,label,xmin,xmax):
    """This method takes the list of mean and rms values from the uniform maps and
    plots the histograms. 
    """
    from matplotlib import pyplot as plt
    mean = numpy.mean(array)
    rms = numpy.std(array)
    plt.hist(array, bins=numpy.linspace(xmin,xmax,50))
    plt.title("%s %s"%(label,gem))
    #good for fwhm
    #plt.axis([xmin,xmax,0.0, 57])
    plt.axis([xmin,xmax,0.0, 45])
    plt.xlabel("%s"%label)
    plt.ylabel("Counts/bin")
    plt.grid(True)
    plt.text(4000,40, "Mean:%d rms:%d"%(mean,rms), fontsize=16)
    #plt.text(0.25,40, "Mean:%.3f rms:%.3f"%(mean,rms), fontsize=16)
    plt.savefig("%s_hist_%s.png"%(label,gem))
    plt.show()


def get_mean_rms(gem, fwhm_hist, main_peak_hist):
    #Brutally copied from pha.py, needs to be done better!
    num_side_bins=20
    half_size=7.5
    num_points=200

    x_bins = numpy.linspace(-half_size, half_size, num_side_bins)
    y_bins = numpy.linspace(-half_size, half_size, num_side_bins)

    fwhm_bin_list = []
    peak_bin_list = []
    
    for i,x in enumerate(x_bins):
        for j,y in enumerate(y_bins):
            bin_fwhm = fwhm_hist.GetBinContent(j+1,i+1)
            fwhm_bin_list.append(bin_fwhm)
            main_peak = main_peak_hist.GetBinContent(j+1,i+1)
            peak_bin_list.append(main_peak)

    fwhm_bin_array = numpy.array(fwhm_bin_list)
    fwhm_mean = numpy.mean(fwhm_bin_array)
    fwhm_rms = numpy.std(fwhm_bin_array)
    
    peak_bin_array = numpy.array(peak_bin_list)
    peak_mean = numpy.mean(peak_bin_array)
    peak_rms = numpy.std(peak_bin_array)
            
    #Good values for fwhm hist
    #plot_hist(fwhm_bin_array,gem,label='EnergyRes',xmin=0.15,xmax=0.35)
    #Good values for main peak hist
    plot_hist(peak_bin_array,gem,label='MainPeak',xmin=3000,xmax=5500)
    
def plot_gem_summary(plot_hists=False):
    """This method is only useful for generating a 3x4 canvas with the uniformity maps of the
    10 GEMs we tested. 
    """
    gem_list = ['gem_1413','gem_1414','gem_1415','gem_1416','gem_1417','gem_1418',
                'gem_1419','gem_1420','gem_1421','gem_1422']
    
    cFWHM = ROOT.TCanvas("FWHM","FWHM",1050, 1400)
    cFWHM.Divide(3,4)

    cPeak = ROOT.TCanvas("Peak","Peak",1050, 1400)
    cPeak.Divide(3,4)
    
    pool = []
    for i,gem in enumerate(gem_list):
        root_file_path = '%s_uniformities.root'%gem
        f = ROOT.TFile(root_file_path)
        fwhm_label = "FWHM_%s"%gem
        main_peak_label = "MainPeak_%s"%gem
        h = f.Get(fwhm_label)
        g = f.Get(main_peak_label)
        if plot_hist:
            get_mean_rms(gem,h,g)
        cFWHM.cd(i+1)

        h.Draw('colz')
        h.SetTitle(fwhm_label)
        h.GetYaxis().SetTitle("fBaricenterY[0]")
        h.GetXaxis().SetTitle("fBaricenterX[0]")
        h.SetMinimum(0.15)
        h.SetMaximum(0.35)
        cFWHM.Update()
        
        cPeak.cd(i+1)
        g.Draw('colz')
        g.SetTitle(main_peak_label)
        g.GetYaxis().SetTitle("fBaricenterY[0]")
        g.GetXaxis().SetTitle("fBaricenterX[0]")
        g.SetMaximum(5500)
        g.SetMinimum(3000)
        cPeak.Update()
        
        pool.append(h)
        pool.append(g)
        pool.append(f)
        pool.append(fwhm_label)
        pool.append(main_peak_label)
    raw_input()

    
#for gem in ['gem_1419']:
#for gem in dictionary.keys():
#    parse_and_fit(gem, useExpression=True)
#    parse_and_plot2d(gem)


#plot_gem_summary()
#gem ='gpd20_run25_26_27'
#gem = 'gpd20_run958'
gem = 'gpd20_run59'
#gem ='gpd20_run23_24'
#gem = 'gpd20_run10_11'
#gem = 'gpd20_run28_29_30'
#gaintrend_gpd20_run28_29_30.data
#gaintrend_file = 'gaintrend_gpd20.data'
#gaintrend_gpd20_run23_24.data
#gaintrend_file = 'gaintrend_gpd20_run09_10_11.data'

#fit_gain_trend(gaintrend_file,gem)
#parse_and_fit(gem, useExpression=True)
#parse_and_plot2d(gem)


#label = 'gpd20'
#run_list = [923,924,925,926,927,928,929,930,931,958,959,960,961,962,963,964,965,966,972,973]
run_list = [929,930,931,958,959,960,961,962,963,964,965,966,972,973,974,975]
file_path_list = []
for run in run_list:
    root_file_path = '/data/xpe/xperecon/out/001_0000%s_data_TH5_outputfile.root'%run
    file_path_list.append(root_file_path)
    
    #make_maps(run,label)
    
good_region_cut = "fBaricenterX[0]>=2&&fBaricenterX[0]<=5&&fBaricenterY[0]>=-6&&fBaricenterY[0]<=-3"
bad_region_cut = "fBaricenterX[0]>=2&&fBaricenterX[0]<=5&&fBaricenterY[0]>=3&&fBaricenterY[0]<=6"
center_region_cut = "fBaricenterX[0]>=-2&&fBaricenterX[0]<=2&&fBaricenterY[0]>=-2&&fBaricenterY[0]<=2"
variable_gain_region_cut = "fBaricenterX[0]>=-6&&fBaricenterX[0]<=-3&&fBaricenterY[0]>=-6&&fBaricenterY[0]<=-3"

label = 'run929_975_tbin5'
tbining = 5

g_region_filepath = run_trending(file_path_list,good_region_cut,'gpd20_small_good_region_%s'%label,tbining=tbining)

b_region_filepath = run_trending(file_path_list,bad_region_cut,'gpd20_small_bad_region_%s'%label,tbining=tbining)


vg_region_filepath = run_trending(file_path_list,variable_gain_region_cut,'gpd20_small_variable_gain_region_%s'%label,tbining=tbining)

center_region_filepath = run_trending(file_path_list,center_region_cut,'gpd20_small_center_region_%s'%label,tbining=tbining)

plot_trending_by_zone(g_region_filepath, b_region_filepath, vg_region_filepath,center_region_filepath)




