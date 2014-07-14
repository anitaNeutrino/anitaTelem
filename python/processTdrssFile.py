import sys
from pprint import pprint
import tdrss



def main():
 
    if(len(sys.argv)<2):
        print __file__+' <telemfile>'
        sys.exit()

    telemName=sys.argv[1]
    print telemName


    tf=tdrss.TdrssFile(telemName)
    tf.loop()





if __name__ == "__main__":
    main()
