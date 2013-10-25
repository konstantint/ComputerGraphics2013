/**
 * MTAT.03.015 Computer Graphics.
 * Practice session 8: Textures
 */
#include <vector>
using namespace std;

/**
 * A mesh of N x N points. Each point simulates a basic spring.
 * Connections between points are added into the spring equation to
 * simulate wavy behaviour. Not sure it is physically correct, but looks OK.
 */
class WaveMesh {
public:
    int size;
    vector< vector<float> > pos;
    vector< vector<float> > v;
    vector< vector<float> > a;
    float k, f, c; // Spring power, friction, connectivity
    WaveMesh(int size, float k = 0.05, float f = 0.0025, float c = 0.05):
        size(size), pos(size+1), v(size+1), a(size+1),
        k(k), f(f), c(c) {
        for (int i = 0; i <= size; i++) pos[i].resize(size+1);
        for (int i = 0; i <= size; i++) v[i].resize(size+1);
        for (int i = 0; i <= size; i++) a[i].resize(size+1);
    }

    void touch(float x, float y, float a=1) {
        int i = (x + 1.0)/2.0*(size+1);
        int j = (1.0 - y)/2.0*(size+1);
        if (i < 0) i = 0;
        if (i > size+1) i = size+1;
        if (j < 0) j = 0;
        if (j > size+1) j = size+1;
        v[i][j] += a;
    }

    void step(float dt) {
        for (int i = 0; i <= size; i++) {
            for (int j = 0; j <= size; j++) {
                a[i][j] = -k*pos[i][j] - f*v[i][j];
                if (i < size) a[i][j] += c*(pos[i+1][j] - pos[i][j]);
                if (i > 0)    a[i][j] += c*(pos[i-1][j] - pos[i][j]);
                if (j < size) a[i][j] += c*(pos[i][j+1] - pos[i][j]);
                if (j > 0)    a[i][j] += c*(pos[i][j-1] - pos[i][j]);
            }
        }
        for (int i = 0; i <= size; i++) {
            for (int j = 0; j <= size; j++) {
                pos[i][j] += v[i][j]*dt;
                v[i][j] += a[i][j];
            }
        }
    }

    void _emitNormal(int i, int j) {
        float dx = (i < size) ? (pos[i+1][j] - pos[i][j]) : (pos[i][j] - pos[i-1][j]);
        float dy = (j < size) ? (pos[i][j] - pos[i][j+1]) : (pos[i][j-1] - pos[i][j]);
        glNormal3f(-dx, -dy, 1);
    }

    void draw() {
        float dx = 2.0/size;
        float dy = 2.0/size;
        for (int j = 0; j < size; j++) {
            float top = 1 - j*dy;
            float bottom = top - dy;
            glBegin(GL_TRIANGLE_STRIP);
                _emitNormal(0, j);
                glVertex3f(-1, top, pos[0][j]);
                _emitNormal(0, j+1);
                glVertex3f(-1, bottom, pos[0][j+1]);
                for (int i = 1; i <= size; i++) {
                    _emitNormal(i, j);
                    glVertex3f(-1.0 + dx*i, top, pos[i][j]);
                    _emitNormal(i, j+1);
                    glVertex3f(-1.0 + dx*i, bottom, pos[i][j+1]);
                }
            glEnd();
        }
    }
};
