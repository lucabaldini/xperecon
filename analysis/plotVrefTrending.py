import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as dlt
import argparse
import os
import datetime

formatter = argparse.ArgumentDefaultsHelpFormatter
parser = argparse.ArgumentParser(formatter_class=formatter)
parser.add_argument('infile', type=str, nargs='+',
                    help='the input log file')
parser.add_argument('-l', '--label', type=str, default='Vref',
                    help='labels for plots')
parser.add_argument('-s', '--saveplot', action='store_true',
                    help='flag to save final plot')
args = parser.parse_args()

Label    = args.label
SavePlot = args.saveplot

time_list = [] 
vref_v_list  = []
vref_adc_list = []

for fpath in args.infile:
    if os.path.isfile(fpath):
        f = open(fpath)
        print fpath
    else:
        print ('Non existing file %s' %f)

    for l in f:
        if "Reading reference voltage" in l:
            #print l.strip('\n')
            tt = l.split('[DEBUG] (')[-1].split(') Reading ')[0]
            #(Tue Feb 28 17:50:38 2017) -> '%a %b %d %H:%M:%S %Y'
            time_list.append(datetime.datetime.strptime(tt,'%a %b %d %H:%M:%S %Y'))
            adc = int(l.split('voltage...')[-1].split(' ADC')[0])
            v   = float(l.split('counts (')[-1].split('V)')[0])
            vref_v_list.append(v)
            vref_adc_list.append(adc)

vref_v_list  = np.array(vref_v_list)
vref_adc_list = np.array(vref_adc_list, 'd')
n = len(vref_v_list)
v_ave = vref_v_list.sum()/n
v_rms = np.sqrt(((vref_v_list-v_ave)**2).sum()/(n*(n-1)))
adc_ave = vref_adc_list.sum()/n
adc_rms = np.sqrt(((vref_adc_list-adc_ave)**2).sum()/(n*(n-1)))
print "Number of entries %d" %(n)
print "Vref V ave = %f rms= %f" %(v_ave, v_rms)
print "Vref ADC ave = %f rms= %f" %(adc_ave, adc_rms)
dates = dlt.date2num(time_list)

plt.figure(1,figsize=(16,8))
#plt.subplot(211)
#plt.plot(np.arange(n), vref_v_list, '.')
##plt.axis([0, n+10, v_ave-5*v_rms, v_ave+5*v_rms])
#plt.axis([0, n+10, 0.78, 0.79])
#plt.subplot(212)
plt.ylabel('Vref ADC around average')
#plt.xlabel('setting index')
plt.plot_date(dates, vref_adc_list, '.', label="ave = %.2f rms= %.2f" %(adc_ave, adc_rms))

#plt.plot(np.arange(n),vref_adc_list, '.', label="ave = %.2f rms= %.2f" %(adc_ave, adc_rms))
#plt.axis([0, n+10, adc_ave-10, adc_ave+10])
plt.axis([min(dates), max(dates), adc_ave-10, adc_ave+10])
plt.gcf().autofmt_xdate()
plt.legend(loc='best')
         
if SavePlot:
    plt.savefig("figure_Vref_%s.png" % Label)
plt.show()
