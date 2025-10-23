from shiboken6 import wrapInstance
from PySide6 import QtCore, QtWidgets
import maya.cmds as cmds
from maya import OpenMayaUI
from maya.app.general.mayaMixin import MayaQWidgetDockableMixin

def get_maya_win():
    win_ptr = OpenMayaUI.MQtUtil.mainWindow()
    return wrapInstance(int(win_ptr), QtWidgets.QMainWindow)

def delete_workspace_control(control):
    if cmds.workspaceControl(control, q=True, exists=True):
        cmds.workspaceControl(control, e=True, close=True)
        cmds.deleteUI(control, control=True)

class StandardSurfaceTextureApplicatorUI(MayaQWidgetDockableMixin, QtWidgets.QWidget):
    WINDOW_NAME = "Standard Surface Texture Applicator"

    def __init__(self, parent=None):

        super(self.__class__, self).__init__(parent=parent)
        self.mayaMainWindow = get_maya_win()
        self.setObjectName(self.__class__.WINDOW_NAME)
        self.setWindowFlags(QtCore.Qt.Window)
        self.setWindowTitle(self.TOOL_NAME)
        self.resize(300, 300)
        
        self.setLayout(QtWidgets.QVBoxLayout())
