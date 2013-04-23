#include <iostream>
#include <QTime>
#include "MapThread.h"
#include "MapObj.h"

 GLThread::GLThread(MapObj *gl) 
: QThread(), glw(gl)
{
doRendering = true;
doResize = false;
}
    
void GLThread::stop()
{
doRendering = false;
}
    
void GLThread::resizeViewport(const QSize &size)
{
w = size.width();
h = size.height();
doResize = true;
}    
    
void GLThread::run()
{
//srand(QTime::currentTime().msec());
//rotAngle = rand() % 360;
//            
//glw->makeCurrent();
//glMatrixMode(GL_PROJECTION);
//glLoadIdentity();        
//glOrtho(-5.0, 5.0, -5.0, 5.0, 1.0, 100.0);
//glMatrixMode(GL_MODELVIEW);
//glViewport(0, 0, 200, 200);
//glClearColor(0.0, 0.0, 0.0, 1.0);
//glShadeModel(GL_SMOOTH);
//glEnable(GL_DEPTH_TEST);
    
//while (doRendering) {
//    if (doResize) {
//        glViewport(0, 0, w, h);
//        doResize = false;
//    }
//    // Rendering code goes here
//    glw->swapBuffers();
//    msleep(40);
//}

while(1)
{
}


}