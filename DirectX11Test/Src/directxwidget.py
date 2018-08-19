from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore
import DirectX11
import random
import numpy
import pyautogui

class DirectXObject(object):
    def Render(self):
        pass
    def Update(self):
        pass

class DirectXWidget(QOpenGLWidget):
    objectList = []
    def __init__(self, parent = None):
        super(DirectXWidget, self).__init__(parent)
        sampleDesc = DirectX11.RTSampleDesc()
        sampleDesc.count = 1
        sampleDesc.quality = 0
        x = 1050
        y = 630
        self.swapChain = DirectX11.SwapChain(self.winId().ascapsule(), x, y, sampleDesc)
        self.viewport = DirectX11.Viewport(0,0,x, y)
        self.camera = DirectX11.Camera(x,y,3.14/4.0,1.0,1000.0)
        self.camera.SetPos(30,30,30)
        self.camera.SetAt(0,0,0)
        self.camera.SetUp(0,1,0)
        # self.renderer = DirectX11.Polygon2DRenderer(8,8)
        # self.renderer.Begin()
        # vertices = [0.0]
        # for i in range(7):
        #     point = random.random()
        #     vertices += [point]    
        # self.renderer.Set(0, vertices)
        # self.renderer.End()
        # self.renderCount = 8
        # self.renderTopology = DirectX11.PrimitiveTopology.LINE_STRIP
        self.timerId = self.startTimer(16)
        
    # def GetRendere(self):
    #     return self.renderer

    def paintGL(self):
        self.swapChain.Clear(0, 0, 0, 0)
        self.viewport.Activate()
        self.camera.Activate()
        # self.renderer.Render(self.renderCount,self.renderTopology)
        for render in DirectXWidget.objectList:
            render.Render()
        self.swapChain.Present(1)
        
    def CameraUpdate(self):
        
        pass
        
    def Update(self):
        for update in DirectXWidget.objectList:
            update.Update()
        
    #タイマーイベントで定期的に描画内容更新
    #http://melpystudio.blog82.fc2.com/blog-entry-153.html?sp
    def timerEvent(self, event):
        self.Update()
        self.paintGL()