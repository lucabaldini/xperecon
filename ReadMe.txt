PIXI (v6.1):  Updated version of XPERECON (Aug. 22, 2016)

Can be run under Linux and MCOSX

Linux
To be compiled with g++ 4.2 use "make"
--------------------------------------------------
TODO: Implement readings of fitsfile with rms and average of pixels peds.
      Pixel threshold  not fixed but varying on the base of the rms of the pixel pedestal
      Implement readings of fitsfile with coefficients to correct for pixel gain non-uniformity


Pixy can run in  INTERACTIVE mode or in BATCH mode;

BATCH mode:
USAGE: ./Pixy [Options] [Arguments]

Options: 
  -h, --help              Print this help 
  -v, --verbose <vlevel>  Change verbosity level
  -b, --batch-mode        Run in batch mode

Arguments: 
  dataFileList            Data files list
  nEvents [default: ALL]  Max n. of event to be processed

The output data are saved in a root file (in the same directory of the input file) with name:
     inputfilename_THxx__outputfile.root (xx is the value of the threshold).

EXAMPLE: ./Pixy -b datafiles.list nEvents

It starts reading the config.dat file to load user's configuration parameters and, if any, overwrite the default values.
Defaults:

	SmallCircleRadius = 1.5
	WideCircleRadius = 3.5
	WeightLenghtScale = 0.05
	PixelThreshold = 9  (fixed value in ADC counts or number of rms, according to the FixThrFlag setting)
	FixThrFlag = 1   (0 - if rms from peds)

---- ***************************************************************************************************** ----

INTERACTIVE mode:
USAGE: ./Pixy

--------------------------------------------------
How to use the 'Pixy Control Panel'
--------------------------------------------------	

     Select the input data file via the top menu File.     
     The data can be both real data (extension .mdat) or Monte Carlo simulation (automatically recognized 
     from extension .root). 
     The data are intended pedestal subtracted.
     Once the datafile has been selected, browsing through the filesystem, it must be set the number of events 
     in the file to to be analysed (from event number - to event number; All stands for "up to the end of file").
     For track reconstructio is necessary to set:
 
     The Pixel Threshold   -   Default value: 9
     The SmallCircleRadius -   Default value: 1.5
     The WideCircleRadius  -   Default value: 3.5	
     The WeightLenghtScale -   Default value: 0.05

     Check "Fixed" button to apply the same theshold value, in ADC counts, to all pixels; uncheck it to apply a variable threshold to each pixel,
     in "number" of sigmas of noise. Pedestal sigmas and means map is automatically loaded if "fixed" button is unchecked. (Sigmas and means 
     are stored in a fitsfile).

     Check "Header On" for input data file from Roma DAQ system 
     Check "Save Raw Signal" to save raw data in RawSignals.root (to be done only for few events, in window mode)

     The offline analysis and track reconstruction starts by by clicking the button 'Analyze data'.
     The output data are saved in a root file (in the same directory of the input file) with name:
     inputfilename_THxx.root (xx is the value of the threshold).

     The result of the analysis or the sigle event tracks can be displayed by clicking the button "Display data'
     that opens the filesystem browser to select the output rootfile to display.

     
--------------------------------------------------
How to use the 'Pixy Data Diplay'
--------------------------------------------------

     The Control Panel is divided in two parts:
     left side to display the tracks, the right side to display the histograms of the main variables
     (Cluster Pulseheight, Cluster Size, Cluster Multiplicity, Angular Distribution, Major 3rd Moment, Skewness, 
     shape=2ndMomentMax/2ndMomentMin, Baricenters, Impact Points, Trigger Window Size) that have passed the cuts.
     
     The cuts (on pulse height, cluster number, cluster size, shape, 3rd Mom, trigger window) 
     are intended applied to all events outside the range defined from the lower value (left entry) 
     to the upper value (right entry) except for "Cut PH in" and "M3 In Range" that cut all events inside 
     the given range.
     The three different types of geometrical cuts (circular, elliptical and rectangular) are in AND with the
     other cuts.

     The Angular Distributions (iteration 0 and 1) are fitted with a cos2 function and the modulation factor is computed.	

     By repeatedly clicking the button "Select Track", the tracks that have passed all the cuts are displayed
     one after the other, in the order they are found in the output root file.

     Raw events can be displayed by clicking "Display Raw Data" if the check button "Save Raw Signal" 
     on the Main Pixy Control Panel.
     
     The button "Display S/W circle" draws, on the track canvas, the two circles which define the region where 
     the impact point is searched.

     It is possible to zoom the track by decreasing the "Zoom Window" value (after the value is changed, click on 
     "Display Track" to zoom the same event).

     Check "Color Scale Hot" to have the track displayed in red color scale or "Cold" for blue color scale 
     (hexagonal pixels have same size)are drown 

     Any autput root files from generated by the Pixy Analysis Package can be re-loaded by using the "Directory Browsing"
     button of the Pixy data Display.




//// OLD UPDATES

25/05/07 - in TEventDisplay_new_v3.cpp: events to display can be selected on the basis of 
	   the Pulse Height from the right Panel "Cuts on events" used in the "Display Clusters Plots".

29/05/07 - in TCluster_hex.cpp method CalcProperties fixed cluster algorithm (no holes foreseen)

09/07/07 - in TEventDisplay_new_v3.cpp implemeted methods InitializeEntries(), WriteInfoFile(),
           ReadInfoFile() to save last display settings in file infodisplay.dat
	   Added cut also on trigger window size.

10/07/07 - added the radio button configuration (M3 In/Out Range) in the file infodisplay.dat

11/07/07 - with the "select event" in the Event navigator only the events that pass ALL the CUTS 
           of the "Display Clusters Plots" are selected.

13/07/07 - Implemented palette hot red amd cold blue to display single event with coor levels 
   	   in a new canvas. Palettes are selectable wuth check button. 

27/07/07 - Added variable fCluLenght[nclusters] in ClustersData.root. fCluLenght 
	   is the lenght of the cluster along the  MAJOR axis.

30/07/07 - Implemented browsing of directory in TEventDisplay_new_v3.cpp to selected different 
     	   output file respect to the default ClustersData.root.
	   The file name MUST contain the string ClustersData (ClustersData*.root)  to understand that 
	   refers to ANALIZED DATA)If ClustersData.root doen't exist in current directory, a menu 
	   popup to browse a new location for output file. The entry "Events" allows to select part 
	   of the file. After changed the num of events, hitting the enter key the slider maximum 
	   is set to that value and also histograms in the cut selection are filled to that maximum.
	   
05/09/07 - For old data stream in window mode without timestamp and other stuff, use ReadROFile(totnev) 
 	   method in TMainGUI_new_v2.cpp instead of ReadROInew(totnev). It is mandatory to edit, 
      	   uncomment the command and recompile the code.

14/10/09 - Geometric cuts changed from Baricenter to on Impact point map.
           In TEventDisplay_new_v3.cxx line 1134 changed to: sprintf(DummyChar,"%3.3f".Angle1)
           Changed histogram name in: line 997  Clusters Map --> BaricenterMap
                                      line 1003 Impact Point Map --> ImpactPointMap
				      line 1009 CumulativeHisto --> CumulativeHitMap
				
18/10/09 - Added selection of events from - to certain event number. Added new entry (from: ) in Main Control Panel for 
	   starting ev. 

19/04/10 - Added in TMainGUI_new_v2.cpp new method ClearArrays(NbClusters) to clear cluster vectors which fill the
           ROOT tree ClustersData.root; added in Tree fUpixel[i][j] and fVpixel[i][j] with U,V coordinates of each 
           pixels in the clusters (now all clusters of an event can be plot directly with the Draw method starting the 
           TreeViewer.

          
