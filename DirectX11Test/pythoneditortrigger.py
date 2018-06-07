from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore
from pythonCodeEditWindow import Ui_python_code_editor
from StdOutputHooker import *

class CodeEditWindow(QWidget):
    def __init__(self, parent=None):
        super(CodeEditWindow, self).__init__(parent)
        self.ui = Ui_python_code_editor()
        self.ui.setupUi(self)        
        self.setWindowFlags(QtCore.Qt.Window)
        self.console = StdOutputHooker()
    #この関数をオーバーライドしておけば、メインウインドウが操作できなくなる
    # def show(self):
    #     self.exec()
        
class PythonEditorTrigger(QLabel):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.window = CodeEditWindow(self)
        self.window.ui.execute_button.clicked.connect(self.OutputBrowser)
        
    def mouseDoubleClickEvent(self, event):
        self.window.show()

    def OutputBrowser(self):
        text = self.window.console.GetConsoleText()
        self.window.ui.text_browser.setText(text)