PIXI (v5):  Updated version of Polarim_v4   					17 Jan. 2008
Can be run under Windows and Linux
---------
Win32
To be compiled with VC++.NET(7.1): in the VC++ prompt use "make -f MAkefileWin"
Need ROOT libraries for WindowsXP/NT/w2000 with VC++ 7.1 (works also VC++6 and VC++8.0)
Tested with ROOT 5.17/08

Linux
To be compiled with g++ 3.4 use "make -f MakefileLinux"
--------------------------------------------------
TODO: ANALYSIS of pedestals 

--------------------------------------------------
How to use the 'Pixy' analysis program.
--------------------------------------------------	

1.1) Selection and analysis of the pedestal.
     You will need to re-do this operation every time you change 'pedestal'. 
     Select the pedestal using the browser and analyze it.
     At this point Pixy saves a root file with the mean and the RMS of every channel. 
     You don't need to reload a pedestal if you are using the same pedestal 
     (in particular for MC simulation you will do this operation only once).
..................................................
1.2) Selects the data file and analyze it:
     The data file are both real data and Monte Carlo simulation(automatically recognized 
     from extension .root). 
     The default extension of the file is '.dat' for full frame data. In this case data 
     are not pedestal subtracted so pedestal should analized before and then "Subtract pedetal" 
     must be checked for data analysis.  
     For data with NEW read-out in window mode extention MUST BE '.mdat'. These data are intended 
     pedestal subtracted.
     Once you have selected the datafile with the data browser, you can fix the maximum 
     number of events to analyze (default value is All), and then, by clicking 'analyze' 
     you can start the analysis. At the end of the analysis a file named 'ClustersData.root' 
     is saved in the Pixy dir (and overwritten each time). 
     The button 'Display Data' opens a new window from where you can chose the value of 
     the cuts to apply.
     Display data loads the file ClustersData.root and fill the histograms with the events. 
     A fit with a cos2 is done and the modulation factor is computed.
..................................................
	Remark:
	- the algorithm for track reconstruction is in the class TCluster_hex.cpp 
	  TCluster.* 'clusterizes' the event and computes all the momenta of the cluster.
	- TPixy.h contains all the constant definitions, including all the parameters for 
	  the reconstruction of the impact point and of the photo-electron emission direction.


//// UPDATES

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

20/04/10 - Important NOTE!!! The coordinate system XY of Pixy is ROTATED 90 degree respect to Massimo system, 
	   i.e. the coordinate along the column of 300 pixels (at 50 micron pitch) is X for Massimo, Y for me; this
           means that for example in reading the ROI edge I read in sequence Ymin, Ymax, Xmin, Xmax WHILE in the data 
           structure of Massimo is written Xmin,Xmax,Ymin,Ymax! 
           --->>> For this reason in the method ReadROInew(Int_t numEv) it has been swapped ix with iy (respect 
	   to previous versions). Swap has been done just for coherence with the coord. system BUT nothing has changed 
           in practice in the results.
          
