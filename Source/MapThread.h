#ifndef __GLTHREAD_H
#define __GLTHREAD_H

#include <QThread>
#include <QSize>

class MapObj;

class GLThread : public QThread
{
public:
    GLThread(MapObj *glWidget);
    void resizeViewport(const QSize &size);
    void run();
    void stop();
        
private:
    bool doRendering;
    bool doResize;
    int w;
    int h;
    int rotAngle;
    MapObj *glw;
};

#endif // __GLTHREAD_H