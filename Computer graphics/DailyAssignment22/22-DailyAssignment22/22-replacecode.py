gVisibles = [True,False,False,False] # visible flags for slerp, interpolateZYXEuler, interpolateRotVec, interpolateRotMat
def render(ang):
    global gCamAng, gCamHeight
    global gVisibles
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

    glEnable(GL_DEPTH_TEST)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, 1, 1,10)

    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(5*np.sin(gCamAng),gCamHeight,5*np.cos(gCamAng), 0,0,0, 0,1,0)

    drawFrame() # draw global frame

    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glEnable(GL_RESCALE_NORMAL) # rescale normal vectors after transformation and before lighting to have unit length

    glLightfv(GL_LIGHT0, GL_POSITION, (1.,2.,3.,1.))
    glLightfv(GL_LIGHT0, GL_AMBIENT, (.1,.1,.1,1.))
    glLightfv(GL_LIGHT0, GL_DIFFUSE, (1.,1.,1.,1.))
    glLightfv(GL_LIGHT0, GL_SPECULAR, (1.,1.,1.,1.))

    # start orientation
    # ZYX Euler angles: rot z by -90 deg then rot y by 90 then rot x by 0
    euler1 = np.array([-1.,1.,0.])*np.radians(90)   # in ZYX Euler angles
    R1 = ZYXEulerToRotMat(euler1)  # in rotation matrix
    rv1 = log(R1)   # in rotation vector

    # end orientation
    # ZYX Euler angles: rot z by 0 then rot y by 0 then rot x by 90
    euler2 = np.array([0.,0.,1.])*np.radians(90)   # in ZYX Euler angles
    R2 = ZYXEulerToRotMat(euler2)  # in rotation matrix
    rv2 = log(R2)   # in rotation vector

    # t is repeatedly increasing from 0.0 to 1.0
    t = (ang % 90) / 90.

    M = np.identity(4)

    # slerp
    if gVisibles[0]:
        R = slerp(R1, R2, t)
        glPushMatrix()
        M[:3,:3] = R
        glMultMatrixf(M.T)
        drawCubes(1.)
        glPopMatrix()

    # interpolation between rotation vectors
    if gVisibles[1]:
        R = interpolateRotVec(rv1, rv2, t)
        glPushMatrix()
        M[:3,:3] = R
        glMultMatrixf(M.T)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (0.,1.,0.,1.))
        drawCubes(.75)
        glPopMatrix()

    # interpolation between Euler angles
    if gVisibles[2]:
        R = interpolateZYXEuler(euler1, euler2, t)
        glPushMatrix()
        M[:3,:3] = R
        glMultMatrixf(M.T)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (0.,0.,1.,1.))
        drawCubes(.5)
        glPopMatrix()

    # interpolation between rotation matrices
    if gVisibles[3]:
        R = interpolateRotMat(R1, R2, t)
        glPushMatrix()
        M[:3,:3] = R
        glMultMatrixf(M.T)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (0.,0.,1.,1.))
        drawCubes(.25)
        glPopMatrix()

    glDisable(GL_LIGHTING)


def key_callback(window, key, scancode, action, mods):
    global gCamAng, gCamHeight
    global gVisibles
    # rotate the camera when 1 or 3 key is pressed or repeated
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
            gVisibles[0] = not gVisibles[0]
        elif key==glfw.KEY_S:
            gVisibles[1] = not gVisibles[1]
        elif key==glfw.KEY_D:
            gVisibles[2] = not gVisibles[2]
        elif key==glfw.KEY_F:
            gVisibles[3] = not gVisibles[3]
        elif key==glfw.KEY_Z:
            gVisibles[:] = [False]*4
        elif key==glfw.KEY_X:
            gVisibles[:] = [True]*4
