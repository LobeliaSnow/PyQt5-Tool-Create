import sys
import os
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

from MainWindow import Ui_MainWindow

# watchdog ... ファイル監視
# scoket ...
# ctype ...
# https://qiita.com/Chan_moro/items/9b0105e4c18bb76ed4e9 ... 未来感じる
# exec ... 文字列からpython実行
# execfile ... ファイルからpython実行
# http://d.hatena.ne.jp/moriyoshi/20091214/1260779899 ... DirectXを使用する際に必要になるのではないか?
# http://owa.as.wakwak.ne.jp/zope/docs/Python/BindingC/ ...

def Func():
    print("A")

class Main(QMainWindow):
    def __init__(self, parent=None):
        super(Main, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.Func)
        sys.stdout = open("temp.bin","w")

    def Func(self):
        self.ui.textBrowser.setText("")
        text = self.ui.textEdit.toPlainText()
        try:
            exec(text)
        except Exception:
            e = sys.exc_info()[1]
            self.ui.textBrowser.setText(e.args[0])
            return
        self.StdoutToTextBrowserOutput()
        # print(text)
    #標準出力に来た内容はファイルディスクリプタに保存しているのでその内容をQtのテキストビュアーへ出力
    def StdoutToTextBrowserOutput(self):
        sys.stdout.close()
        temp = open("temp.bin","r")
        outBuffer = temp.read()
        self.ui.textBrowser.setText(outBuffer)
        temp.close()
        sys.stdout = open("temp.bin","w")

def main():
    app = QApplication(sys.argv)
    window = Main()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
