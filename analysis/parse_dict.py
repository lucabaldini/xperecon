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
    file_path = '/data/xpe/xperecon/out/001_0000%s_data_TH5_outputfile.root'%run
    _label = '%s_run%s'%(label,run)
    output_file_path = '/data/xpe/xperecon/analysis/%s_uniformities.root'%(_label)
    cmd = 'python plotGEMUniformMap.py %s -o %s -l %s '%(file_path, output_file_path, _label)
    os.system(cmd)
    parse_and_plot2d(_label)


def plot_trending_by_zone(g_file_path,b_file_path):
    time, g_peak, g_fwhm, g_timeErr, g_peakErr, g_fwhmErr = \
                        numpy.loadtxt(g_file_path, delimiter='\t', unpack=True)

    time, b_peak, b_fwhm, b_timeErr, b_peakErr, b_fwhmErr = \
                        numpy.loadtxt(b_file_path, delimiter='\t', unpack=True)

    
    from matplotlib import pyplot as plt
    import matplotlib
    matplotlib.rcParams.update({'font.size': 18})
    fig = plt.figure(figsize=(15,10),facecolor='w')
    plt.errorbar(time,g_fwhm, xerr=0, yerr=g_fwhmErr, label='Good region', marker='o',ls='none')
    g_pol_coef = numpy.polyfit(time[time>60], g_fwhm[time>60], deg=1)
    plt.plot(time[time>60], numpy.polyval(g_pol_coef, time[time>60]),lw=2,color='b')
    plt.text(40, 0.4, 'y = %sx + %s'%(g_pol_coef[0],g_pol_coef[1]),fontsize=18, color='b')
    
    plt.errorbar(time,b_fwhm, xerr=0, yerr=b_fwhmErr, label='Bad region',color='r', marker='o',ls='none')
    b_pol_coef = numpy.polyfit(time[time>60], b_fwhm[time>60], deg=1)
    plt.plot(time[time>60], numpy.polyval(b_pol_coef, time[time>60]),lw=2,color='r')
    plt.text(40, 0.35, 'y = %sx + %s'%(b_pol_coef[0],b_pol_coef[1]),fontsize=18, color='red')
    
    plt.xlabel('Time [hr]')
    plt.ylabel('DeltaE/E (%)')
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

    #offset = 1.48550e+09 #offset for gem 1422#1.48596e+09 Offset for gem 1422 
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
    root_file_path = '%s_uniformities.root'%label

    canvas = ROOT.TCanvas("%s"%label,"%s"%label,1200,700)
    canvas.Divide(2,1)
    canvas.cd(1)
    f = ROOT.TFile(root_file_path)
    fwhm_label = "FWHM_%s"%label
    main_peak_label = "MainPeak_%s"%label
    #c1 = ROOT.TCanvas(fwhm_label)
    fwhm = f.Get(fwhm_label)
    min_val, max_val,mean_val,rms_val = get_stats(fwhm)
    fwhm_title = "%s FWHM %.3f-%.3f <%.3f> rms:%.3f"%(label,min_val, max_val,mean_val,rms_val)
    fwhm.SetTitle(fwhm_title)
    fwhm.GetYaxis().SetTitle("fBaricenterY[0]")
    fwhm.GetXaxis().SetTitle("fBaricenterX[0]")
    #fwhm.SetMaximum(0.28)
    #fwhm.SetMinimum(0.17)
    fwhm.Draw("colz")
    canvas.Update()
    #c1.SaveAs("%s.png"%fwhm_label)
    #raw_input()
    #c2 = ROOT.TCanvas(main_peak_label)
    canvas.cd(2)
    peak = f.Get(main_peak_label)
    p_min_val, p_max_val,p_mean_val,p_rms_val = get_stats(peak)
    main_peak_title = "%s MainPeak %.3f-%.3f <%.3f> rms:%.3f"%(label,p_min_val, p_max_val,p_mean_val,p_rms_val)
    peak.SetTitle(main_peak_title)
    peak.GetYaxis().SetTitle("fBaricenterY[0]")
    peak.GetXaxis().SetTitle("fBaricenterX[0]")
    peak.Draw("colz")
    #c2.SaveAs("%s.png"%main_peak_label)
    canvas.SaveAs("%s_maps.png"%label)
    #raw_input()

def plot_gem_summary():
    gem_list = ['gem_1413','gem_1414','gem_1415','gem_1416','gem_1417','gem_1418',
                'gem_1419','gem_1420','gem_1421','gem_1422']
    
    cFWHM = ROOT.TCanvas("FWHM","FWHM",1400, 1050)
    cFWHM.Divide(4,3)

    cPeak = ROOT.TCanvas("Peak","Peak",1400, 1050)
    cPeak.Divide(4,3)
    
    pool = []
    for i,gem in enumerate(gem_list):
        root_file_path = '%s_uniformities.root'%gem
        f = ROOT.TFile(root_file_path)
        fwhm_label = "FWHM_%s"%gem
        main_peak_label = "MainPeak_%s"%gem
        h = f.Get(fwhm_label)
        g = f.Get(main_peak_label)
        cFWHM.cd(i+1)

        h.Draw('colz')
        h.SetTitle(fwhm_label)
        h.GetYaxis().SetTitle("fBaricenterY[0]")
        h.GetXaxis().SetTitle("fBaricenterX[0]")
        cFWHM.Update()
        
        cPeak.cd(i+1)
        g.Draw('colz')
        g.SetTitle(main_peak_label)
        g.GetYaxis().SetTitle("fBaricenterY[0]")
        g.GetXaxis().SetTitle("fBaricenterX[0]")
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
#run_list = [923,924,925,926,927,928,929,930,931,958,959]
#for run in run_list:
 #   make_maps(run,label)


b_region_filepath = "gaintrend_gpd20_small_bad_region.data"
g_region_filepath = "gaintrend_gpd20_small_good_region2.data"

plot_trending_by_zone(g_region_filepath, b_region_filepath)
