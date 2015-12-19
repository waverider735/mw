__author__ = "John Hemingway"
__copyright__ = "Copyright April, 2015, 2mi Solutions Inc."
__credits__ = ["John Hemingway"]
__license__ = "GPL"
__version__ = "1.0.1"
__maintainer__ = "John Hemingway"
__email__ = "jhemingway@sentex.net"
__status__ = "Development"

import serial, sys, datetime, time, os
from dgps import DGPSData

#open a serial connection to the virtual USB port and set timeout to 0
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0)
i=0;
outfile="null";
if len(sys.argv)>1:
    outfile=sys.argv[len(sys.argv)-1];
    outfptr=open(outfile,'w');
while True:
    os.system('clear')
    line = ser.readline()
    d = DGPSData(line.split('\n'))     
    line2 = str(datetime.datetime.now()) + ',' + d.toString() 
    print(line2)        
    if outfile!="null":    
        outfptr.write("%s" % line2);
    time.sleep(0.595);

outfptr.close();



