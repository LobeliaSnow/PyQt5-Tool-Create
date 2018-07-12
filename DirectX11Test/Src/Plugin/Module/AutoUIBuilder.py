#.uiファイル作成、変更検知で.pyへ変換
import os
import time

from watchdog.events import FileSystemEventHandler
from watchdog.observers import Observer

BASE_DIR = os.path.abspath(os.path.dirname(__file__))

def GetExtension(filename):
    return os.path.splitext(filename)[-1].lower()

# if GetExtension(event.src_path) in ('.jpg','.png','.txt'):
def BuildUI(file_path):
    name,ext = os.path.splitext(file_path)
    os.system("pyuic5 {src} -o {fn}".format(src = file_path, fn = name+".py"))

class ChangeHandler(FileSystemEventHandler):    
    #UIファイルが作られたとき
    def on_created(self, event):
        #ディレクトリの場合無視
        if event.is_directory:
            return
        if GetExtension(event.src_path) in ('.ui'):
            BuildUI(event.src_path)
    #変更検知
    def on_modified(self, event):
        if event.is_directory:
            return
        if GetExtension(event.src_path) in ('.ui'):
            BuildUI(event.src_path)
    #削除検知
    def on_deleted(self, event):
        pass

def main():
 while 1:
        event_handler = ChangeHandler()
        observer = Observer()
        observer.schedule(event_handler, BASE_DIR, recursive=True)
        observer.start()
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            observer.stop()
        observer.join()

    
if __name__ in '__main__':
    main()
    pass
