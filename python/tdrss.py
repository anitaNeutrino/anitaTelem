import struct


class TdrssFile:

    def __init__(self,fileName):
        self.f = open(fileName,"rb")


    def handleScience(self,buffer):
        print len(buffer)
        count=0
        

        
    
    def loop(self):
        count=0;
        bytesread=self.f.read()
        print len(bytesread)


        while(count<2000) :
            startHdr=struct.unpack('H',bytesread[count+0:count+2])[0]
            auxHdr=struct.unpack('H',bytesread[count+2:count+4])[0]
            idHdr=struct.unpack('H',bytesread[count+4:count+6])[0]
            if (startHdr==0xf00d and auxHdr==0xd0cc) :
                bufCnt=struct.unpack('H',bytesread[count+6:count+8])[0]
                numBytes=struct.unpack('H',bytesread[count+10:count+12])[0]
                print "Got buffer "+str(bufCnt)+" num bytes "+str(numBytes)
                self.handleScience(bytesread[count+12:count+numBytes+12])
                count+=20+numBytes


