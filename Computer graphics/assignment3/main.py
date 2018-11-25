import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
from OpenGL.arrays import vbo
import ctypes

gCamAng = 0.
gCamHeight = 1.
gZoom = 45
gVarr = []

polygon_mode = True

def render(ang):
    global gCamAng, gCamHeight, gzoom, polygon_mode
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

    glEnable(GL_DEPTH_TEST)
    if polygon_mode:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    else:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
    glMatrixMode(GL_PROJECTION) # use projection matrix stack for projection transformation for correct lighting
    glLoadIdentity()
    gluPerspective(gZoom, 1, 1,10)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(5*np.sin(gCamAng),gCamHeight,5*np.cos(gCamAng), 0,0,0, 0,1,0)


    glEnable(GL_LIGHTING)   # try to uncomment: no lighting
    glEnable(GL_LIGHT0)
    glEnable(GL_LIGHT1)
    # light position

    lightPos = (1.,2.,3.,1.)    # try to change 4th element to 0. or 1.
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos)

    # light intensity for each color channel
    ambientLightColor = (.1,.1,.1,1.)
    diffuseLightColor = (1.,1.,1.,1.)
    specularLightColor = (1.,1.,1.,1.)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColor)
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLightColor)

    lightPos_2 = (-2.,3.,3.,1.)
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos_2)

    ambientLightColor_2 = (.3,.3,.3,1.)
    diffuseLightColor_2 = (.5,.5,.5,1.)
    specularLightColor_2 = (.3,.3,.3,1.)
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightColor_2)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightColor_2)
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularLightColor_2)

    # material reflectance for each color channel
    diffuseObjectColor = (1.,1.,0.,1.)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuseObjectColor)
    drawObject_glDrawArray()
    glDisable(GL_LIGHTING)




def drawObject_glDrawArray():
    global gVarr
    varr = np.array(gVarr)

    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_DOUBLE,6*varr.itemsize,varr)
    glVertexPointer(3,GL_DOUBLE,6*varr.itemsize,ctypes.c_void_p(varr.ctypes.data+3*varr.itemsize))
    glDrawArrays(GL_TRIANGLES,0,int(varr.size/6))


def key_callback(window, key, scancode, action, mods):
    global gCamAng, gCamHeight, gZoom, polygon_mode
    if action==glfw.PRESS or action==glfw.REPEAT:
        if key==glfw.KEY_1:
            gCamAng += np.radians(-10)
        elif key==glfw.KEY_3:
            gCamAng += np.radians(10)
        elif key==glfw.KEY_2:
            gCamHeight += .1
        elif key==glfw.KEY_W:
            gCamHeight += -.1
        elif key==glfw.KEY_A:
            gZoom -= 1
        elif key==glfw.KEY_S:
            gZoom += 1
        elif key==glfw.KEY_Z:
            polygon_mode = not polygon_mode

def drop_callback(window, paths):
    global gVarr

    try:
        obj_file = open(" ".join(paths), 'r')
    except FileNotFoundError:
        print('정확한 파일 주소의 파일을 올려주세요.')


    print("file name : " + str(paths))
    num_face = 0
    num_thr_ver_face = 0
    num_four_ver_face = 0
    num_other_ver_face = 0
    vertex = []
    normal = []
    varr = []

    index_num = 0
    for line in obj_file:
        if line[:2] == "v ":
             index1 = line.find(" ") + 1
             index2 = line.find(" ", index1 +1)
             index3 = line.find(" ", index2 + 1)
             v = (float(line[index1:index2]), float(line[index2:index3]), float(line[index3:-1]))
             vertex.append(v)
        elif line[:2] == "vn":
             index1 = line.find(" ") + 1
             index2 = line.find(" ", index1 + 1)
             index3 = line.find(" ", index2 + 1)
             v = (float(line[index1:index2]), float(line[index2:index3]), float(line[index3:-1]))
             normal.append(v)
        elif line[:2] == "f ":
             v_start_x = line.find(" ") + 1
             v_end_x = line.find("/", v_start_x)
             n_start_x = line.find("/", v_end_x + 1) + 1
             n_end_x = line.find(" ", n_start_x)
             v_start_y = line.find(" ", n_end_x) + 1
             v_end_y = line.find("/", v_start_y)
             n_start_y = line.find("/", v_end_y + 1) + 1
             n_end_y = line.find(" ", n_start_y)
             check_vert = 3
             start_point = n_end_y + 1

             while line.find(" ", start_point) != -1:
                 v_start_z = line.find(" ", n_end_y) + 1
                 v_end_z = line.find("/", v_start_z)
                 n_start_z = line.find("/", v_end_z + 1) + 1
                 n_end_z = line.find(" ", n_start_z)

                 v = (int(line[v_start_x:v_end_x])-1, int(line[v_start_y:v_end_y])-1, int(line[v_start_z:v_end_z])-1)
                 v2 = (int(line[n_start_x:n_end_x])-1, int(line[n_start_y:n_end_y])-1, int(line[n_start_z:n_end_z])-1)

                 for i in range(0,3):
                     varr.append(normal[v2[i]])
                     varr.append(vertex[v[i]])

                 v_start_y = v_start_z
                 v_end_y = v_end_z
                 n_start_y = n_start_z
                 n_end_y = n_end_z

                 check_vert += 1
                 start_point = n_end_y + 1

             v_start_z = line.find(" ", n_end_y) + 1
             v_end_z = line.find("/", v_start_z)
             n_start_z = line.find("/", v_end_z + 1) + 1

             v = (int(line[v_start_x:v_end_x])-1, int(line[v_start_y:v_end_y])-1, int(line[v_start_z:v_end_z])-1)
             v2 = (int(line[n_start_x:n_end_x])-1, int(line[n_start_y:n_end_y])-1, int(line[n_start_z:-1])-1)

             for i in range(0,3):
                 varr.append(normal[v2[i]])
                 varr.append(vertex[v[i]])

             num_face += 1
             if check_vert == 3:
                 num_thr_ver_face += 1
             elif check_vert == 4:
                 num_four_ver_face += 1
             elif check_vert > 4:
                 num_other_ver_face += 1

    obj_file.close()
    gVarr = varr

    print("number of all face :", num_face)
    print("number of 3vertices face:", num_thr_ver_face)
    print("number of 4vertices face:", num_four_ver_face)
    print("number of other vertices face:", num_other_ver_face)


def main():

    if not glfw.init():
        return
    window = glfw.create_window(640,640,'2014004411', None,None)
    if not window:
        glfw.terminate()
        return
    glfw.make_context_current(window)
    glfw.set_key_callback(window, key_callback)
    glfw.set_drop_callback(window, drop_callback)
    glfw.swap_interval(1)

    count = 0
    while not glfw.window_should_close(window):
        glfw.poll_events()
        ang = count % 360
        render(ang)
        count += 1
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
			

