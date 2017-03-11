
import numpy

from scipy.optimize import curve_fit
from matplotlib import pyplot as plt

#plt.ion()

file_path = 'gaintrend_gpd20_run923_992.data'
try:
    t, peak, fwhm, dt, dpeak, dfwhm = numpy.loadtxt(file_path, unpack=True)
except:
    t, peak, fwhm, rate, dt, dpeak, dfwhm, drate = numpy.loadtxt(file_path, unpack=True)
    

def falling_exp(x, C, D, tau, x0):
    """
    """
    return C + D*numpy.exp(-(x - x0)/tau)


def rising_exp(x, C, D, tau, x0):
    """
    """
    return C - D*numpy.exp(-(x - x0)/tau)


_mask1 = (t > 35.)*(t < 210)
_t1 = t[_mask1]
_peak1 = peak[_mask1]
_rate1 = rate[_mask1]

p0 = (4500, 1000, 48, 0.)
popt1, pcov1 = curve_fit(falling_exp, _t1, _peak1, p0)
print("popt1",popt1)

_mask2 = (t > 215.)*(t < 310)
_t2 = t[_mask2]
_peak2 = peak[_mask2]
_rate2 = rate[_mask2]

p0 = (5000, 500, 215, 48.)
popt2, pcov2 = curve_fit(rising_exp, _t2, _peak2, p0)
print("popt2", popt2)

_mask3 = (t > 310.)*(t < 337)
_t3 = t[_mask3]
_peak3 = peak[_mask3]
_rate3 = rate[_mask3]

p0 = (4400, 500, 16, 310.)
popt3, pcov3 = curve_fit(falling_exp, _t3, _peak3, p0)
print("popt3", popt3)


_mask4 = (t > 340)*(t < 358)
_t4 = t[_mask4]
_peak4 = peak[_mask4]
_rate4 = rate[_mask4]

p0 = (4200, 300, 10, 340.)
popt4, pcov4 = curve_fit(falling_exp, _t4, _peak4, p0)
print("popt4", popt4)


plt.figure()
plt.grid()
plt.errorbar(t, peak, dpeak, fmt='o')
plt.plot(_t1, falling_exp(_t1, *popt1), lw=2, color='r')
plt.plot(_t2, rising_exp(_t2, *popt2), lw=2, color='g')
plt.plot(_t3, falling_exp(_t3, *popt3), lw=2, color='blue')
plt.plot(_t4, falling_exp(_t4, *popt4), lw=2, color='magenta')
plt.text(100, 5000, 'C = %.1f, $\\tau$ = %.1f h' % (popt1[0], popt1[2]),
         color='r')
plt.text(270, 5000, 'C = %.1f, $\\tau$ = %.1f h' % (popt2[0], popt2[2]),
         color='g')
plt.text(200, 4500, 'C = %.1f, $\\tau$ = %.1f h' % (popt3[0], popt3[2]),
         color='blue')
plt.text(220, 4300, 'C = %.1f, $\\tau$ = %.1f h' % (popt4[0], popt4[2]),
         color='magenta')



r = _rate2.mean(), _rate1.mean(),  _rate3.mean(), _rate4.mean()
C =  popt2[0], popt1[0],popt3[0], popt4[0] 
tau = popt2[2], popt1[2],  popt3[2], popt4[2]
dtau = numpy.sqrt(pcov2.diagonal()[2]), numpy.sqrt(pcov1.diagonal()[2]), numpy.sqrt(pcov3.diagonal()[2]), numpy.sqrt(pcov4.diagonal()[2])

x = numpy.arange(min(r), max(r), 1)

plt.figure()
plt.grid()
plt.plot(r, C, 'o')
plt.xlabel('Rate [Hz]')
plt.ylabel('C [ADC counts]')
popt_C, pcov_C = curve_fit(falling_exp, r, C, (5000, 100, 10, 1) )
plt.plot(x, falling_exp(x, *popt_C))
print("popt_C", popt_C)

plt.figure()
plt.grid()
plt.errorbar(r, tau, dtau, fmt='o')
plt.xlabel('Rate [Hz]')
plt.ylabel('$\\tau$ [h]')
popt_tau, pcov_tau = curve_fit(falling_exp, r, tau, (70, 10, 1, 1) )
plt.plot(x, falling_exp(x, *popt_tau))
print("popt_tau", popt_tau)

plt.show()
