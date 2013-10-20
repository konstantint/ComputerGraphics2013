/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 7: Lighting
 *
 * A simplistic "random smooth surface" renderer.
 */
#include <vector>
using std::vector;

// Uses NURBS to render a smooth surface.
// The theory behind that will be covered in the
// upcoming lectures.
class RandomSurface {
public:
    vector<float> ctl_points;
    vector<float> knots;
    int n;
    float minX, maxX, minY, maxY, minZ, maxZ;
    // N must be at least 4
    RandomSurface(int n, float minX=-1, float maxX=1, float minY=-1, float maxY=1, float minZ=-1, float maxZ=1):
        n(n), minX(minX), maxX(maxX), minY(minY), maxY(maxY), minZ(minZ), maxZ(maxZ) {
        ctl_points = vector<float>(n*n*3);
        knots = vector<float>(8);
        srand(1);
        for (int i = 0; i < 8; i++) knots[i] = i;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                ctl_points[(i*n+j)*3 + 0] = (float)i/(n-1)*(maxX-minX) + minX;
                ctl_points[(i*n+j)*3 + 1] = (float)j/(n-1)*(maxY-minY) + minY;
                ctl_points[(i*n+j)*3 + 2] = (float)rand()/RAND_MAX*(maxZ - minZ) + minZ;
            }
        }
    }
    void draw(bool show_points = false) {
        GLUnurbsObj* surface = gluNewNurbsRenderer();
        gluNurbsProperty(surface, GLU_SAMPLING_TOLERANCE, 100.0);
        gluNurbsProperty(surface, GLU_DISPLAY_MODE, GLU_FILL);

        glEnable(GL_AUTO_NORMAL);
        for (int i = 0; i < n-3; i++) {
            for (int j = 0; j < n-3; j++) {
                gluBeginSurface(surface);
                gluNurbsSurface(surface, 8, &knots[0], 8, &knots[0],
                                n * 3, 3, &ctl_points[(n*i + j)*3],
                                4, 4, GL_MAP2_VERTEX_3);
                gluEndSurface(surface);
        }
        }
        gluDeleteNurbsRenderer(surface);
        glDisable(GL_AUTO_NORMAL);

        if (show_points) {
            glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT | GL_ENABLE_BIT);
            glPointSize(5.0);
            glDisable(GL_LIGHTING);
            glColor3f(1.0, 1.0, 1.0);
            glBegin(GL_POINTS);
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        glVertex3f(ctl_points[(i*n+j)*3],
                                   ctl_points[(i*n+j)*3 + 1],
                                   ctl_points[(i*n+j)*3 + 2]);
            glEnd();
            for (int i = 0; i < n; i++) {
                glBegin(GL_LINE_STRIP);
                for (int j = 0; j < n; j++)
                    glVertex3f(ctl_points[(i*n+j)*3],
                               ctl_points[(i*n+j)*3 + 1],
                               ctl_points[(i*n+j)*3 + 2]);
                glEnd();
            }
            for (int j = 0; j < n; j++) {
                glBegin(GL_LINE_STRIP);
                for (int i = 0; i < n; i++)
                    glVertex3f(ctl_points[(i*n+j)*3],
                               ctl_points[(i*n+j)*3 + 1],
                               ctl_points[(i*n+j)*3 + 2]);
                glEnd();
            }
            glPopAttrib();
        }
    }
};
