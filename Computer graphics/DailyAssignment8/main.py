from OpenGL.GL import *
from OpenGL.GLU import *
import glfw
import numpy as np

gComposedM = np.identity(4)
count = 0

def drawTriangle():
	glColor3ub(255, 255, 255)
	glBegin(GL_TRIANGLES)
	glVertex3fv(np.array([.0,.5,0.]))
	glVertex3fv(np.array([.0,.0,0.]))
	glVertex3fv(np.array([.5,.0,0.]))
	glEnd()

def render(M, camAng):

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	glEnable(GL_DEPTH_TEST)

	glLoadIdentity()

	glOrtho(-1,1,-1,1,-1,1)

	gluLookAt(.1*np.sin(camAng),.1,.1*np.cos(camAng),0,0,0,0,1,0)

	glBegin(GL_LINES)
	glColor3ub(255, 0, 0)
	glVertex3fv(np.array([0.,0.,0.]))
	glVertex3fv(np.array([1.,0.,0.]))
	glColor3ub(0, 255, 0)
	glVertex3fv(np.array([0.,0.,0.]))
	glVertex3fv(np.array([0.,1.,0.]))
	glColor3ub(0, 0, 255)
	glVertex3fv(np.array([0.,0.,0]))
	glVertex3fv(np.array([0.,0.,1.]))
	glEnd()

	glMultMatrixf(M.T)
	drawTriangle()

def key_callback(window, key, scancode, action, mods):
	
	global gComposedM
	global count

	if action==glfw.PRESS or glfw.REPEAT:
		if key == glfw.KEY_Q:
			gComposedM = np.array([[1.,0.,0.,-0.1],[0.,1.,0.,0.],[0.,0.,1.,0.],[0.,0.,0.,1.]]) @ gComposedM
		elif key == glfw.KEY_E:
			gComposedM = np.array([[1.,0.,0.,0.1],[0.,1.,0.,0.],[0.,0.,1.,0.],[0.,0.,0.,1.]]) @gComposedM
		elif key == glfw.KEY_A:
			th = np.radians(-10)
			gComposedM = gComposedM @ np.array([[np.cos(th),0.,np.sin(th),0.],[0.,1.,0.,0.],[-np.sin(th),0.,np.cos(th),0.],[0.,0.,0.,1.]])
		elif key == glfw.KEY_D:
			th = np.radians(10)
			gComposedM = gComposedM @ np.array([[np.cos(th),0.,np.sin(th),0.],[0.,1.,0.,0.],[-np.sin(th),0.,np.cos(th),0.],[0.,0.,0,1.]])
		elif key == glfw.KEY_W:
			th = np.radians(-10)
			gComposedM = gComposedM @ np.array([[1.,0.,0.,0.],[0.,np.cos(th),-np.sin(th),0.],[0.,np.sin(th),np.cos(th),0.],[0.,0.,0.,1.]])
		elif key == glfw.KEY_S:
			th = np.radians(10)
			gComposedM = gComposedM @ np.array([[1.,0.,0.,0.],[0.,np.cos(th),-np.sin(th),0.],[0.,np.sin(th),np.cos(th),0.],[0.,0.,0.,1.]])
		elif key == glfw.KEY_1:
			count -=10
		elif key == glfw.KEY_3:
			count +=10

def main():
	global gComposedM
	global count

	if not glfw.init():
		return
	window = glfw.create_window(480,480,"2014004411", None,None)

	if not window:
		glfw.terminate()
		return
	glfw.make_context_current(window)
	glfw.set_key_callback(window, key_callback)

	while not glfw.window_should_close(window):
		glfw.poll_events()
		# rotate 60 deg about x axis

		camAng = np.radians(count % 360)

		render(gComposedM, camAng)

		glfw.swap_buffers(window)
	glfw.terminate()

if __name__ == "__main__":
	main()
