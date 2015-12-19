""" Description: Data logger python script for collecting EEG data from mindwave headset by Neurosky.
    Runs from the terminal and loggs data to a file named on the command line. Connects to mindwave via bluetooth
    and to redboard (arduino Uno) via serial connection
    Data can be saved when logging is done by pressing ctrl-c to exit the program
    Data is saved in a comma delimited format for importing into other analysis programs
    Last update: June 14, 2015

""" 

__author__ = "John Hemingway"
__copyright__ = "Copyright June, 2015, 2mi Solutions Inc."
__credits__ = ["John Hemingway"]
__license__ = "GPL"
__version__ = "1.0.1"
__maintainer__ = "John Hemingway"
__email__ = "jhemingway@sentex.net"
__status__ = "Development"

import datetime,sys, textwrap, time, os
from MindwaveDataPoints import RawDataPoint, EEGPowersDataPoint, MWDataPoint, MWDataPoints, PoorSignalLevelDataPoint, AttentionDataPoint, MeditationDataPoint, BlinkDataPoint
from mindwavemobile.MindwaveDataPointReader import MindwaveDataPointReader

if __name__ == '__main__':
    
    datapoint = MWDataPoint(0,0,0,0,0,0)
    datapoints = MWDataPoints()   
    i = 0
    cycle = 1
    str1 = ''
    timenow = ''
            
    #open the datafile
    outfile="null";
    if len(sys.argv)>1:
        outfile=sys.argv[len(sys.argv)-1]
        f=open(outfile,'a')       
#    f.write("File opened successfully!\n")
        
    mindwaveDataPointReader = MindwaveDataPointReader()
    mindwaveDataPointReader.start()
    if (mindwaveDataPointReader.isConnected()):    
        while(True):
            
            dataPoint = mindwaveDataPointReader.readNextDataPoint()
            if (not dataPoint.__class__ is RawDataPoint):
                if (dataPoint.__class__ is PoorSignalLevelDataPoint):
                    datapoint.poorsignal = dataPoint
                if (dataPoint.__class__ is AttentionDataPoint):
                    datapoint.attention = dataPoint
                if (dataPoint.__class__ is MeditationDataPoint):
                    datapoint.meditation = dataPoint
                if (dataPoint.__class__ is BlinkDataPoint):
                    datapoint.blink = dataPoint
                if (dataPoint.__class__ is RawDataPoint):
                    datapoint.raw = dataPoint
                if (dataPoint.__class__ is EEGPowersDataPoint):
                    datapoint.eeg = dataPoint
                    timenow = time.time()
#                    timenow = datetime.datetime.now().strftime("%H:%M:%S.%f")
                    if ( i == 0):
                        datapoints.add(datapoint)
                    os.system('clear')
                    print("%f,%s" % (timenow, datapoint))
#                    f.write("%d,%s\n" % (i, datapoint))
                    i += 1
                if (i == cycle):                   
                    for item in datapoints.mwdata:
                        f.write("%f,%s\n" % (timenow, item))
                        i = 0
                        datapoint.reset()
                        datapoints = MWDataPoints()

    else:
        print(textwrap.dedent("""\
            Exiting because the program could not connect
            to the Mindwave Mobile device.""").replace("\n", " "))
    f.close()
