import sys
import os
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

from MainWindow import Ui_MainWindow

#watchdog ... ファイル監視
#scoket ... 
#ctype ... 
#https://qiita.com/Chan_moro/items/9b0105e4c18bb76ed4e9 ... 未来感じる
#exec ... 文字列からpython実行
#execfile ... ファイルからpython実行

def TestFunc():
    print("func__")
class Main(QMainWindow):
    def __init__(self, parent=None):
        super(Main, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.Func)
    def Func(self):
       text = self.ui.textEdit.toPlainText()
       exec(text)
       #print(text)
    

def main():
    app = QApplication(sys.argv)
    window = Main()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
