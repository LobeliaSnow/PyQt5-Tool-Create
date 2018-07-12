import Main
from StdOutputHooker import *
from PyQt5 import QtGui, QtCore, QtWidgets
import DirectX11
import directxwidget
import test
# http://dungeonneko.hatenablog.com/entry/2015/06/07/134938
# import cppmod
# from ctypes import pythonapi, py_object
# import ctypes
# import DirectX11Project

#ツールで使用される関数軍
#スクリプト作成時に使えるようにと

#ツール上の出力ウインドウの文字を空にする
def OutputWindowClear():
    StdOutputHooker.Clear()
    
    
#ファイルパスが返ってくる、選択がなければ空文字
def SaveFileDialog(parent = None, dialog_name = "ファイルを保存", ext_list="全ての形式(*.*)", filter = "", view_path=""):
    fd = QtWidgets.QFileDialog()
    fp = fd.getSaveFileName(parent, dialog_name, view_path, ext_list, filter)
    return fp

#文字列として投げられるパイソンコードを実行
#実行結果の文字列を返す
def ExecutePythonCode(text):
    try:
        exec(text)
    except Exception:
        e = sys.exc_info()[1]
        return e.args[0]
    # text = StdOutputHooker.GetConsoleText()
    # return text
