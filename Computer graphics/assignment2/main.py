import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

gCamAng = np.radians(0)
gCamHeight = 0.1

def drawFrame():
	glBegin(GL_LINES)
	glColor3ub(255,0,0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([1., 0., 0.]))
	glColor3ub(0,255,0)
	glVertex3fv(np.array([0., 0., 0.]))
	glVertex3fv(np.array([0., 1., 0.]))
	glColor3ub(0,0,255)
	glVertex3fv(np.array([0.,0.,0]))
	glVertex3fv(np.array([0.,0.,1.]))
	glEnd()



def drawCube():
	glBegin(GL_QUADS)
	glVertex3f( 1.0, 1.0,-1.0)
	glVertex3f(-1.0, 1.0,-1.0)
	glVertex3f(-1.0, 1.0, 1.0)
	glVertex3f( 1.0, 1.0, 1.0)

	glVertex3f( 1.0,-1.0, 1.0)
	glVertex3f(-1.0,-1.0, 1.0)
	glVertex3f(-1.0,-1.0,-1.0)
	glVertex3f( 1.0,-1.0,-1.0)

	glVertex3f( 1.0, 1.0, 1.0)
	glVertex3f(-1.0, 1.0, 1.0)
	glVertex3f(-1.0,-1.0, 1.0)
	glVertex3f( 1.0,-1.0, 1.0)

	glVertex3f( 1.0,-1.0,-1.0)
	glVertex3f(-1.0,-1.0,-1.0)
	glVertex3f(-1.0, 1.0,-1.0)
	glVertex3f( 1.0, 1.0,-1.0)

	glVertex3f(-1.0, 1.0, 1.0)
	glVertex3f(-1.0, 1.0,-1.0)
	glVertex3f(-1.0,-1.0,-1.0)
	glVertex3f(-1.0,-1.0, 1.0)

	glVertex3f( 1.0, 1.0,-1.0)
	glVertex3f( 1.0, 1.0, 1.0)
	glVertex3f( 1.0,-1.0, 1.0)
	glVertex3f( 1.0,-1.0,-1.0)
	glEnd()

def drawSphere(numLats=12, numLongs=12):
	for i in range(0, numLats + 1):
		lat0 = np.pi * (-0.5 + float(float(i - 1) / float(numLats)))
		z0 = np.sin(lat0)
		zr0 = np.cos(lat0)

		lat1 = np.pi * (-0.5 + float(float(i) / float(numLats)))
		z1 = np.sin(lat1)
		zr1 = np.cos(lat1)

# Use Quad strips to draw the sphere
		glBegin(GL_QUAD_STRIP)

		for j in range(0, numLongs + 1):
			lng = 2 * np.pi * float(float(j - 1) / float(numLongs))
			x = np.cos(lng)
			y = np.sin(lng)
			glVertex3f(x * zr0, y * zr0, z0)
			glVertex3f(x * zr1, y * zr1, z1)

		glEnd()




def render(camAng, gCamHeight, count, count_two):
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE)

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	glEnable(GL_DEPTH_TEST)

	glLoadIdentity()
	glOrtho(-1,1, -1,1, -1,1)
	gluLookAt(.1*np.sin(camAng), gCamHeight, .1*np.cos(camAng), 0,0,0, 0,1,0)

	drawFrame()

	glPushMatrix()
	glTranslatef(-.5 + (count % 360) * .004, 0, 0)

	glPushMatrix()
	glScalef(.2, .2, .2)
	glColor3ub(0, 0, 255)
	drawSphere()
	glPopMatrix()

	glPushMatrix()
	glRotatef(-135+count_two%90,0,0,1)
	glTranslatef(0.1,0,0.23)
	glPushMatrix()
	glScalef(0.03,0.03,0.03)
	glColor3ub(0,255,0)
	drawCube()
	glPopMatrix()

	glPushMatrix()
	glTranslatef(0.08,0.03,0)
	glRotatef(count_two%90,0,0,1)
	glScalef(0.05,0.03,0.03)
	glColor3ub(255,0,0)
	drawCube()
	glPopMatrix()
	glPopMatrix()

	glPushMatrix()
	glRotatef(-45-count_two%90,0,0,1)
	glTranslate(0.1,0,-0.23)
	glPushMatrix()
	glScalef(0.03,0.03,0.03)
	glColor3ub(0,255,0)
	drawCube()
	glPopMatrix()

	glPushMatrix()
	glTranslatef(0.08,0.03,0)
	glRotatef(45-count_two%90,0,0,1)
	glScalef(0.05,0.03,0.03)
	glColor3ub(255,0,0)
	drawCube()
	glPopMatrix()
	glPopMatrix()

	glPushMatrix()
	glRotatef(-45+count_two%90,0,0,1)
	glTranslatef(0,-0.24,-0.05)
	glPushMatrix()
	glScalef(0.03,0.04,0.03)
	glColor3ub(0,255,0)
	drawCube()
	glPopMatrix()

	glPushMatrix()
	glTranslatef(0.01,-0.06,0)
	glScalef(0.05,0.02,0.03)
	glColor3ub(255,0,0)
	drawCube()
	glPopMatrix()
	glPopMatrix()

	glPushMatrix()
	glRotatef(-315-count_two%90,0,0,1)
	glTranslatef(0,-0.24,0.05)
	glPushMatrix()
	glScalef(0.03,0.04,0.03)
	glColor3ub(0,255,0)
	drawCube()
	glPopMatrix()

	glPushMatrix()
	glTranslatef(0.01,-0.06,0)
	glScalef(0.05,0.02,0.03)
	glColor3ub(255,0,0)
	drawCube()
	glPopMatrix()
	glPopMatrix()

	glPopMatrix()

def key_callback(window, key, scancode, action, mods):
	global gCamAng, gCamHeight
# rotate the camera when 1 or 3 key is pressed or repeated
	if action == glfw.PRESS or action == glfw.REPEAT:
		if key == glfw.KEY_1:
			gCamAng += np.radians(-10)
		elif key == glfw.KEY_3:
			gCamAng += np.radians(10)
		elif key==glfw.KEY_2:
			gCamHeight += .05
		elif key==glfw.KEY_W:
			gCamHeight += -.05

def main():
	if not glfw.init():
		return
	window = glfw.create_window(640, 640, '2014004411', None, None)
	if not window:
		glfw.terminate()
		return
	glfw.make_context_current(window)
	glfw.set_key_callback(window, key_callback)
	glfw.swap_interval(1)

	count = 0
	count_two = 0
	check = 0
	while not glfw.window_should_close(window):
		glfw.poll_events()
		render(gCamAng,gCamHeight,count,count_two)
		glfw.swap_buffers(window)
		count +=1
		if check == 0:
			count_two +=1
		elif check == 1:
			count_two -=1
		if count_two == 90:
			check = 1
		elif count_two == 0:
			check = 0

	glfw.terminate()


if __name__ == "__main__":
	main()
