from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore
import DirectX11
import random
import numpy as np
import socket
import struct

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
        self.camera = DirectX11.Camera(x,y,3.14/(180.0/54),1.0,1000.0)
        self.pos = np.array([30.0,30.0,30.0])
        self.right = np.array([1.0,0.0,0.0])
        self.front = np.array([0.0,0.0,1.0])
        self.up = np.array([0.0,1.0,0.0])
        self.at = np.array([0.0,0.0,0.0])        
        self.timerId = self.startTimer(16)
        self.socket = None
        self.client = None
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

    def ConnectMaya(self):
        self.socket = socket.socket()
        self.socket.bind(("",49152))
        self.socket.listen(1)
        self.client, self.clientAddr = self.socket.accept()
        self.client.setblocking(False)
        #self.client.settimeout(0)

    def Recive(self):
        if not self.client:
            return
        try:
            data = self.client.recv(24)
        except:
            return
        data = struct.unpack_from("!ffffff",data)
        self.pos = np.array([data[0],data[1],-data[2]])
        translate = [data[0],data[1],data[2]]
        rotation = [data[3],data[4],data[5]]
        transformer = DirectX11.Transformer()
        transformer.Translation(translate[0],translate[1],translate[2])
        transformer.RotationRollPitchYaw(rotation[0]/180.0*3.14,rotation[1]/180.0*3.14,rotation[2]/180.0*3.14)
        transformer.CalcWorldMatrix()
        # matrix = transformer.GetWorldMatrix()
        matrix = transformer.CalcInverseWorldMatrix()
        front = DirectX11.GetAxisZDirection(matrix)
        self.up = DirectX11.GetAxisYDirection(matrix)
        self.at = [self.pos[0] + front[0]*35.0,self.pos[1] + front[1]*35.0,self.pos[2] + front[2]*35.0]
        
        print(self.at)

    def CameraUpdate(self):                
        if self.client:
            self.camera.SetPos(self.pos[0],self.pos[1],self.pos[2])
            self.camera.SetAt(self.at[0],self.at[1],self.at[2])
            self.camera.SetUp(self.up[0],self.up[1],self.up[2])
            return
        wheel = DirectX11.Mouse.GetWheel()
        if wheel:
            frontMove = self.front * (wheel * 0.2)
            self.pos += frontMove
            self.at += frontMove
        if DirectX11.Mouse.GetKey(0):
            rightMove = self.right * DirectX11.Mouse.GetMoveX() 
            upMove = self.up * DirectX11.Mouse.GetMoveY()
            self.pos += rightMove + upMove
        if DirectX11.Mouse.GetKey(1):
            rightMove = self.right * DirectX11.Mouse.GetMoveX() 
            upMove = self.up * DirectX11.Mouse.GetMoveY()
            self.at += rightMove + upMove
        if DirectX11.Mouse.GetKey(2):
            rightMove = self.right * DirectX11.Mouse.GetMoveX() 
            upMove = self.up * DirectX11.Mouse.GetMoveY()
            self.pos += rightMove + upMove
            self.at += rightMove + upMove
        #カメラ情報
        direct = self.at - self.pos
        self.front = direct / np.linalg.norm(direct)
        self.right = np.cross(self.up,self.front)
        self.up = np.cross(self.front,self.right)
        #カメラ情報更新
        self.camera.SetPos(self.pos[0],self.pos[1],self.pos[2])
        self.camera.SetAt(self.at[0],self.at[1],self.at[2])
        self.camera.SetUp(self.up[0],self.up[1],self.up[2])

    def Update(self):
        DirectX11.Mouse.Update()
        self.CameraUpdate()
        self.Recive()
        for update in DirectXWidget.objectList:
            update.Update()
        
    #タイマーイベントで定期的に描画内容更新
    #http://melpystudio.blog82.fc2.com/blog-entry-153.html?sp
    def timerEvent(self, event):
        self.Update()
        self.paintGL()