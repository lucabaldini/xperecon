import numpy as np
import matplotlib.pyplot as plt
import argparse
import os

formatter = argparse.ArgumentDefaultsHelpFormatter
parser = argparse.ArgumentParser(formatter_class=formatter)
parser.add_argument('infile', type=str,
                    help='the input log file')
parser.add_argument('-l', '--label', type=str, default='Gain test',
                    help='labels for plots')
parser.add_argument('-s', '--saveplot', action='store_true',
                    help='flag to save final plot')
args = parser.parse_args()

Label    = args.label
SavePlot = args.saveplot


if os.path.isfile(args.infile):
    f = open(args.infile)
else:
    print ('Non existing file %s' %f)


vref_v_list  = []
vref_adc_list = []
for l in f:
    if "Reading reference voltage" in l:
        #print l.strip('\n')
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
print "Vref V ave = %f rms= %f" %(v_ave, v_rms)
print "Vref ADC ave = %f rms= %f" %(adc_ave, adc_rms)


plt.figure(1)
#plt.subplot(211)
#plt.plot(np.arange(n), vref_v_list, '.')
##plt.axis([0, n+10, v_ave-5*v_rms, v_ave+5*v_rms])
#plt.axis([0, n+10, 0.78, 0.79])
#plt.subplot(212)
plt.ylabel('Vref ADC around average')
plt.xlabel('setting index')
plt.plot(np.arange(n),vref_adc_list - adc_ave, '.', label="ave = %.2f rms= %.2f" %(adc_ave, adc_rms))
plt.axis([0, n+10, -6, 6])
plt.legend(loc='best')
         

plt.show()
