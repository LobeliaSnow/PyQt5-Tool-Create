from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore
import DirectX11
import random

class RenderObject(object):
    def Render(self):
        pass        

class DirectXWidget(QOpenGLWidget):
    renderList = []
    def __init__(self, parent = None):
        super(DirectXWidget, self).__init__(parent)
        sampleDesc = DirectX11.RTSampleDesc()
        sampleDesc.count = 1
        sampleDesc.quality = 0
        x = 1050
        y = 630
        self.swapChain = DirectX11.SwapChain(self.winId().ascapsule(), x, y, sampleDesc)
        self.material = DirectX11.Material("Simple")
        self.viewport = DirectX11.Viewport(0,0,x, y)
        self.renderer = DirectX11.Polygon2DRenderer(8,8)
        self.renderer.Begin()
        vertices = [0.0]
        for i in range(7):
            point = random.random()
            vertices += [point]    
        self.renderer.Set(0, vertices)
        self.renderer.End()
        self.renderCount = 8
        self.renderTopology = DirectX11.PrimitiveTopology.LINE_STRIP
        
        
    def GetRendere(self):
        return self.renderer

    def initializeGL(self):        
        pass

    def paintGL(self):
        self.swapChain.Clear(0, 0, 0, 0)
        self.viewport.Activate()
        self.material.Activate()
        self.renderer.Render(self.renderCount,self.renderTopology)
        for render in DirectXWidget.renderList:
            render.Render()
        self.swapChain.Present(1)
        pass