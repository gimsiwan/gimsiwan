import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

gangz_1 = 0.
gangx = 0.
gangz_2 = 0.
gCamAng = 0.
gCamHeight = 1.


def createVertexArraySeparate():
	varr = np.array([
		[0,1,0],# v0 normal
		[0.5,0.5,-0.5],# v0 position
		[0,1,0],# v1 normal
		[-0.5,0.5,-0.5],# v1 position
		[0,1,0],# v2 normal
		[-0.5,0.5,0.5],# v2 position

		[0,1,0],# v3 normal
		[0.5,0.5,-0.5],# v3 position
		[0,1,0],# v4 normal
		[-0.5,0.5,0.5],# v4 position
		[0,1,0],# v5 normal
		[0.5,0.5,0.5],# v5 position

		[0,-1,0],# v6 normal
		[0.5,-0.5,0.5],# v6 position
		[0,-1,0],# v7 normal
		[-0.5,-0.5,0.5],# v7 position
		[0,-1,0],# v8 normal
		[-0.5,-0.5,-0.5],# v8 position

		[0,-1,0],
		[0.5,-0.5,0.5],
		[0,-1,0],
		[-0.5,-0.5,-0.5],
		[0,-1,0],
		[0.5,-0.5,-0.5],

		[0,0,1],
		[0.5,0.5,0.5],
		[0,0,1],
		[-0.5,0.5,0.5],
		[0,0,1],
		[-0.5,-0.5,0.5],

		[0,0,1],
		[0.5,0.5,0.5],
		[0,0,1],
		[-0.5,-0.5,0.5],
		[0,0,1],
		[0.5,-0.5,0.5],

		[0,0,-1],
		[0.5,-0.5,-0.5],
		[0,0,-1],
		[-0.5,-0.5,-0.5],
		[0,0,-1],
		[-0.5,0.5,-0.5],

		[0,0,-1],
		[0.5,-0.5,-0.5],
		[0,0,-1],
		[-0.5,0.5,-0.5],
		[0,0,-1],
		[0.5,0.5,-0.5],

		[-1,0,0],
		[-0.5,0.5,0.5],
		[-1,0,0],
		[-0.5,0.5,-0.5],
		[-1,0,0],
		[-0.5,-0.5,-0.5],

		[-1,0,0],
		[-0.5,0.5,0.5],
		[-1,0,0],
		[-0.5,-0.5,-0.5],
		[-1,0,0],
		[-0.5,-0.5,0.5],

		[1,0,0],
		[0.5,0.5,-0.5],
		[1,0,0],
		[0.5,0.5,0.5],
		[1,0,0],
		[0.5,-0.5,0.5],

		[1,0,0],
		[0.5,0.5,-0.5],
		[1,0,0],
		[0.5,-0.5,0.5],
		[1,0,0],
		[0.5,-0.5,-0.5],
		], 'float32')
	return varr


def render(angZ_1, angX, angZ_2):
    global gCamAng, gCamHeight
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

    glEnable(GL_DEPTH_TEST)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1, 1,10)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(5*np.sin(gCamAng),gCamHeight,5*np.cos(gCamAng), 0,0,0, 0,1,0)

    # draw global frame
    drawFrame()

    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glEnable(GL_RESCALE_NORMAL) # rescale normal vectors after transformation and before lighting to have unit length

    # set light properties
    lightPos = (1.,2.,3.,1.)
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos)

    ambientLightColor = (.1,.1,.1,1.)
    diffuseLightColor = (1.,1.,1.,1.)
    specularLightColor = (1.,1.,1.,1.)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColor)
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor)

	
    # ZXZ Euler angles
    zang_1 = np.radians(angZ_1)
    xang = np.radians(angX)
    zang_2 = np.radians(angZ_2)
    M = np.identity(4)
    Rz_1 = np.array([[np.cos(zang_1), -np.sin(zang_1), 0],
                   [np.sin(zang_1), np.cos(zang_1), 0],
                   [0,0,1]])
    Rx = np.array([[1,0,0],
                   [0, np.cos(xang), -np.sin(xang)],
                   [0, np.sin(xang), np.cos(xang)]])
    Rz_2 = np.array([[np.cos(zang_2), -np.sin(zang_2), 0],
                   [np.sin(zang_2), np.cos(zang_2), 0],
                   [0,0,1]])
    M[:3,:3] = Rz_1 @ Rx @ Rz_2
    glMultMatrixf(M.T)


    glScalef(.5,.5,.5)

    # draw cubes
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (.5,.5,.5,1.))
    drawUnitCube_glDrawArray()

    glTranslatef(1.5,0,0)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (1.,0.,0.,1.))
    drawUnitCube_glDrawArray()

    glTranslatef(-1.5,1.5,0)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (0.,1.,0.,1.))
    drawUnitCube_glDrawArray()

    glTranslatef(0,-1.5,1.5)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (0.,0.,1.,1.))
    drawUnitCube_glDrawArray()

    glDisable(GL_LIGHTING)


def drawFrame():
	glBegin(GL_LINES)
	glColor3ub(255, 0, 0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([1., 0., 0.]))
	glColor3ub(0, 255, 0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([0., 1., 0.]))
	glColor3ub(0, 0, 255)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([0., 0., 1.]))
	glEnd()


def drawUnitCube_glDrawArray():
	global gVertexArraySeparate
	varr = gVertexArraySeparate
	glEnableClientState(GL_VERTEX_ARRAY)
	glEnableClientState(GL_NORMAL_ARRAY)
	glNormalPointer(GL_FLOAT,6*varr.itemsize,varr)
	glVertexPointer(3,GL_FLOAT,6*varr.itemsize,ctypes.c_void_p(varr.ctypes.data+3*varr.itemsize))
	glDrawArrays(GL_TRIANGLES,0,int(varr.size/6))



def key_callback(window, key, scancode, action, mods):
	global gangz_1, gangx, gangz_2, gCamAng, gCamHeight

	if action == glfw.PRESS or action == glfw.REPEAT:
		if key == glfw.KEY_A:
			gangz_1 += 10
		elif key == glfw.KEY_Z:
			gangz_1 += -10
		elif key == glfw.KEY_S:
			gangx += 10
		elif key == glfw.KEY_X:
			gangx += -10
		elif key == glfw.KEY_D:
			gangz_2 += 10
		elif key == glfw.KEY_C:
			gangz_2 += -10
		if key == glfw.KEY_1:
			gCamAng += np.radians(-10)
		elif key == glfw.KEY_3:
			gCamAng += np.radians(10)
		elif key == glfw.KEY_2:
			gCamHeight += .1
		elif key == glfw.KEY_W:
			gCamHeight += -.1

		elif key == glfw.KEY_V:
			gangz_1 = 0
			gangz_2 = 0
			gangx = 0
gVertexArrayIndexed = None
gIndexArray = None

def main():
	global gVertexArraySeparate
	global gangz_1, gangx, gangz_2

	if not glfw.init():
		return

	window = glfw.create_window(640, 640, '2014004411', None, None)
	if not window:
		glfw.terminate()
		return

	glfw.make_context_current(window)
	glfw.set_key_callback(window, key_callback)
	glfw.swap_interval(1)
	gVertexArraySeparate = createVertexArraySeparate()

	while not glfw.window_should_close(window):
		glfw.poll_events()
		render(gangz_1, gangx, gangz_2)
		glfw.swap_buffers(window)

	glfw.terminate()


if __name__ == "__main__":
	main()

