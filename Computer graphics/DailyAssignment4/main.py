import glfw
import numpy as np
from OpenGL.GL import *

s = GL_LINE_LOOP

def render():
	global s
	glClear(GL_COLOR_BUFFER_BIT)
	glLoadIdentity()
	glBegin(s)
	x = np.linspace(0,360,13)*np.pi/180
	i = 0
	while(i<12):
		glVertex2f(np.cos(x[i]),np.sin(x[i]))
		i+=1
	glEnd()

def key_callback(window, key, scancode, action, mods):
	global s
	if key==glfw.KEY_1:
		if action==glfw.PRESS:
			s = GL_POINTS
	elif key==glfw.KEY_2:
		if action==glfw.PRESS:
			s = GL_LINES
	elif key==glfw.KEY_3:
		if action==glfw.PRESS:
			s = GL_LINE_STRIP
	elif key==glfw.KEY_4:
		if action==glfw.PRESS:
			s = GL_LINE_LOOP
	elif key==glfw.KEY_5:
		if action==glfw.PRESS:
			s = GL_TRIANGLES
	elif key==glfw.KEY_6:
		if action==glfw.PRESS:
			s = GL_TRIANGLE_STRIP
	elif key==glfw.KEY_7:
		if action==glfw.PRESS:
			s = GL_TRIANGLE_FAN
	elif key==glfw.KEY_8:
		if action==glfw.PRESS:
			s = GL_QUADS
	elif key==glfw.KEY_9:
		if action==glfw.PRESS:
			s = GL_QUAD_STRIP
	elif key==glfw.KEY_0:
		if action==glfw.PRESS:
			s = GL_POLYGON

def main () :
	if not glfw.init():
		return

	window = glfw.create_window(480,480,"2014004411",None, None)
	if not window:
		glfw.terminate()
		return

	glfw.set_key_callback(window, key_callback)
	glfw.make_context_current(window)

	while not glfw.window_should_close(window) :
		glfw.poll_events()

		render()

		glfw.swap_buffers(window)

	glfw.terminate()

if __name__ == "__main__":
	main()
