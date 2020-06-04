import sys, csv, io
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import matplotlib.cm as cm

from PyQt5 import QtCore, QtGui
from PyQt5.QtWidgets import *
import CPyConverter as cpy

# import widgets
from Joystick import Joystick

class Cluster2D(QDialog):

    def __init__(self, parent=None):
        super().__init__(parent)

        layout = QVBoxLayout()
        layout.addWidget(self.create_clusterBox())
        layout.addWidget(self.create_liseBox())
        layout.addWidget(self.create_optionBox())                
        self.setLayout(layout)

        self.closeButton.clicked.connect(self.close)

    def create_clusterBox(self):
        clusterBox = QGroupBox("Clustering 2D")
        
        self.clusterN_label = QLabel("Number of Clusters")
        self.clusterN = QComboBox()
        for i in range(10):
            self.clusterN.addItem(str(i+1))
        self.clusterAlgo_label = QLabel("Clustering Algorithm")
        self.clusterAlgo = QComboBox()        
        self.clusterAlgo.addItem("K-Mean")
        self.clusterAlgo.addItem("Gaussian Mixture Model")        
        self.analyzerButton = QPushButton("Analyze", self)
        self.closeButton = QPushButton("Dismiss", self)

        layoutC = QVBoxLayout()
        layoutC.addWidget(self.clusterN_label)
        layoutC.addWidget(self.clusterN)
        layoutC.addWidget(self.clusterAlgo_label)
        layoutC.addWidget(self.clusterAlgo)        
        layoutC.addWidget(self.analyzerButton)
        layoutC.addStretch(1)
        clusterBox.setLayout(layoutC)
        
        return clusterBox

    def create_liseBox(self):    
        liseBox = QGroupBox("Overlay an Image")        

        self.loadButton = QPushButton("Open File", self)
        self.loadLISE_name = QLineEdit(self)
        self.addButton = QPushButton("Add", self)
        self.deleteButton = QPushButton("Delete", self)                        
        self.alpha_label = QLabel("Transparency Level (20 %)")        
        self.alpha_slider = QSlider(QtCore.Qt.Horizontal, self)
        self.zoom_label = QLabel("Zoom Level (20 %)")        
        self.zoom_slider = QSlider(QtCore.Qt.Horizontal, self)        
        
        self.alpha_slider.setMinimum(0)
        self.alpha_slider.setMaximum(10)
        self.alpha_slider.setTickInterval(1)
        self.alpha_slider.setValue(2)
        self.zoom_slider.setMinimum(0)
        self.zoom_slider.setMaximum(10)
        self.zoom_slider.setTickInterval(1)
        self.zoom_slider.setValue(2)                                

        # joystick layout
        self.joystick = Joystick()
        self.upButton = QToolButton(self)        
        self.upButton.setArrowType(QtCore.Qt.UpArrow)
        self.upButton.setFixedSize(QtCore.QSize(120,20))
        self.downButton = QToolButton(self)        
        self.downButton.setArrowType(QtCore.Qt.DownArrow)
        self.downButton.setFixedSize(QtCore.QSize(120,20))
        self.leftButton = QToolButton(self)        
        self.leftButton.setArrowType(QtCore.Qt.LeftArrow)
        self.leftButton.setFixedSize(QtCore.QSize(20,120))
        self.rightButton = QToolButton(self)        
        self.rightButton.setArrowType(QtCore.Qt.RightArrow)        
        self.rightButton.setFixedSize(QtCore.QSize(20,120))    

        leftLay = QGridLayout()
        leftLay.addWidget(self.leftButton, 0, 0, 3, 1)

        rightLay = QGridLayout()
        rightLay.addWidget(self.rightButton, 0, 0, 3, 1)        

        centerLay = QGridLayout()
        centerLay.addWidget(self.upButton, 0, 0)
        centerLay.addWidget(self.joystick, 1, 0)
        centerLay.addWidget(self.downButton, 2, 0)        

        joyLay = QHBoxLayout()
        joyLay.addLayout(leftLay)
        joyLay.addLayout(centerLay)
        joyLay.addLayout(rightLay)                
        
        lay1 = QVBoxLayout()        
        lay1.addWidget(self.loadButton)
        lay1.addWidget(self.loadLISE_name)
        lay1.addLayout(joyLay)
        lay1.addWidget(self.alpha_label)
        lay1.addWidget(self.alpha_slider)
        lay1.addWidget(self.zoom_label)
        lay1.addWidget(self.zoom_slider)        

        lay2 = QHBoxLayout()
        lay2.addWidget(self.addButton)
        lay2.addWidget(self.deleteButton)
        
        lstlayout = QVBoxLayout()
        lstlayout.addLayout(lay1)
        lstlayout.addLayout(lay2)
        lstlayout.addStretch(1)
        liseBox.setLayout(lstlayout)
        
        return liseBox        

    def create_optionBox(self):        
        optionsBox = QGroupBox("Options")

        layoutC = QVBoxLayout()
        layoutC.addWidget(self.closeButton)                
        layoutC.addStretch(1)
        optionsBox.setLayout(layoutC)
        
        return optionsBox
