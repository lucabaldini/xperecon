

GEM_RUN_DICT = {'GEM1413':{'unif_illumination_runs':[541, 542],
                           'Top HV':'', 'Trending time':17.9,
                           'notes':['Some vertical stripe structures appear to be present in the hitmap.']},
                'GEM1414':{'unif_illumination_runs':[561,562],
                           'Top HV':1475, 'Trending time':16.2,
                            'notes':['']},
                'GEM1415':{'unif_illumination_runs':[574,575,576,577],
                           'Top HV':1475,'Trending time':46.7,
                           'notes':['']},
                'GEM1416':{'unif_illumination_runs':[535, 536],
                           'Top HV':1465, 'Trending time':2.8,
                           'notes':['This GEM did a bake out at 80 degrees C for 21.5 hours.','Using new fixture to set uniform illumination made by Davide.']},

                'GEM1417':{'unif_illumination_runs':[ 410, 411],
                           'Top HV':1475, 'Trending time':6,
                           'notes':['runs 392, 407, 408, 409 are with collimated source']},
                'GEM1418':{'unif_illumination_runs':[424, 425, 426,
                                                     427, 430],
                           'Top HV':1465, 'Trending time':34.8,
                           'notes':['']},
                'GEM1419':{'unif_illumination_runs':[446, 448, 451, 462],
                           'Top HV':1480, 'Trending time':4.2,
                           'notes':['First 3 runs have a hole in the hitmap,found to be due to a whisker in the GEM']},
                'GEM1420':{'unif_illumination_runs':[469],
                           'Top HV':1465, 'Trending time':7,
                           'notes':['Run 470 without Fe source']},
                'GEM1421':{'unif_illumination_runs':[483, 487, 488, 489,
                                                     490, 494, 496],
                           'Top HV':1460, 'Trending time':37.2,
                           'notes':['']},
                'GEM1422':{'unif_illumination_runs':[519, 520, 521],
                           'Top HV':1465, 'Trending time':15.5,
                           'notes':['Using new fixture to set uniform illumination made by Davide.']},
}


if __name__ == '__main__':
    
    dictionary = GEM_RUN_DICT
    for gem in dictionary.keys():
        print "For %s runs are %s Top HV was %s trending time (hr) %s and relevant notes %s\n"%(gem,dictionary[gem]['unif_illumination_runs'],dictionary[gem]['Top HV'],dictionary[gem]['Trending time'],dictionary[gem]['notes'])
       
