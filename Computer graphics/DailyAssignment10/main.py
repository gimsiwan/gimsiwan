import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

gCamAng = 0

def myLookAt(eye, at, up):

	w = (eye - at)/np.sqrt(np.dot(eye-at,eye-at))
	u = np.cross(up,w)/np.sqrt(np.dot(np.cross(up,w),np.cross(up,w)))
	v = np.cross(w,u)
	matrix = np.array([[u[0],u[1],u[2],-np.dot(u,eye)],[v[0],v[1],v[2],-np.dot(v,eye)],[w[0],w[1],w[2],-np.dot(w,eye)],[0,0,0,1]])
	glMultMatrixf(matrix.T)

def render(camAng):
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	glEnable(GL_DEPTH_TEST)

	glLoadIdentity()

	glOrtho(-1,1, -1,1, -10,10)

	eye = np.array([1*np.sin(camAng),1,1*np.cos(camAng)])
	at = np.array([0,0,0])
	up = np.array([0,1,0])

	myLookAt(eye,at,up)

	glBegin(GL_LINES)
	glColor3ub(255, 0, 0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([1., 0., 0.]))
	glColor3ub(0, 255, 0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([0., 1., 0.]))
	glColor3ub(0, 0, 255)
	glVertex3fv(np.array([0., 0., 0]))
	glVertex3fv(np.array([0., 0., 1.]))
	glEnd()


def key_callback(window, key, scancode, action, mods):
	global gCamAng

	if action == glfw.PRESS or action == glfw.REPEAT:
		if key == glfw.KEY_1:
			gCamAng += np.radians(-10)
		elif key == glfw.KEY_3:
			gCamAng += np.radians(10)


def main():
	if not glfw.init():
		return
	window = glfw.create_window(640,640,'2014004411', None, None)

	if not window:
		glfw.terminate()
		return
	glfw.make_context_current(window)
	glfw.set_key_callback(window, key_callback)
	
	while not glfw.window_should_close(window):
		glfw.poll_events()
		render(gCamAng)
		glfw.swap_buffers(window)

	glfw.terminate()


if __name__ == "__main__":
	main()
