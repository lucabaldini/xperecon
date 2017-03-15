

GPD_DICT = {'gpd_20':{'runs_5Hz':[972,973,974,975,976,979,980,981,982],
                      'runs_20Hz':[923,924,925,926,927,928,929,930,931,958,959,960,961,962,963,964,965,966],
                      'runs_50Hz':[983,984,985,986,987,988],
                      'runs_100Hz':[990],
                      'notes':['Source at ~6cm from gpd for 20Hz rate.', 'Run 931 had a monitor crash, pedestals were at 2048. There is a hole in the data',
                               'To get the rate down to 5HZ we added 4 layers of paper on top of the detector','In run 976 there is roughly 1 hour of lost data due to intermittent problem with gpd.','Run 979 has ~8.5Hz rate instead of 5Hz.','To get rate of 50 Hz source was placed closer to the detector, no information on the actual distance.']}
           }

                   
if __name__ == '__main__':

    dictionary = GPD_DICT
    
    for gpd in dictionary.keys():
        print 'GPD', gpd
        for k in dictionary[gpd].keys():
            if 'runs' in k:
                
                print "For %s runs with %s rate are %s \n"%(gpd,k,dictionary[gpd][k])
                        
