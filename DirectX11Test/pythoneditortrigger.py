from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

class PythonEditorTrigger(QLabel):
    def __init__(self, parent=None):
        super().__init__(parent)
    def mouseDoubleClickEvent(self,event):
        
        pass