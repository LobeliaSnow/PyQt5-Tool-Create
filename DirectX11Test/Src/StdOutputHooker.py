import sys
import os
#この辺り調整
class StdOutputHooker:
    lastText = ""   
    def __init__(self):
        # sys.stdout = open("temp.bin", "w")
        pass
    @staticmethod
    def GetConsoleText():
        # sys.stdout.close()
        # temp = open("temp.bin", "r")
        # text = temp.read()
        # temp.close()
        # sys.stdout = open("temp.bin", "w")
        # lastText = text
        # return text
        pass
    @staticmethod
    def Clear():
       print("")
    #    StdOutputHooker.GetConsoleText()