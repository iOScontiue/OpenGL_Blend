#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "math.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//固定管线着色器管理器
GLShaderManager shaderManager;

//容器帮助类，对应图元
GLBatch squareBatch;
GLBatch redBatch;
GLBatch greenBatch;
GLBatch blueBatch;
GLBatch blackBatch;

//移动矩形顶点设置
GLfloat blockSize = 0.2f;
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0f
};

void ChangeSize(int w, int h)
{
    //防止h为0
    if (h == 0) {
        h = 1;
    }
    //动态适配窗口大小
    glViewport(0, 0, w, h);
}

void RenderScene()
{
    //清除缓存区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置四个固定矩形颜色
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 0.5f};
    GLfloat vGreen[] = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat vBlue[] = {0.0f, 0.0f, 1.0f, 1.0f};
    GLfloat vBlack[] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    //绘制四个固定矩形
    //GLT_SHADER_IDENTITY：单元着色器
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    redBatch.Draw();
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGreen);
    greenBatch.Draw();
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlue);
    blueBatch.Draw();
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlack);
    blackBatch.Draw();
    
    //移动矩形设置混合功能
    //开启混合功能
    glEnable(GL_BLEND);
    //设置混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //使用固定管线着色器
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    //绘制
    squareBatch.Draw();
    //关闭混合功能
    glDisable(GL_BLEND);
    
    //后台渲染完成交给前台
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
    //步长
    GLfloat stepSize = 0.025f;
    //以移动矩形框左上角的顶点为参考点
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[7];
    
    //方向控制
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }
    
    //边界处理
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    
    if (blockX > 1.0f-2*blockSize) {
        blockX = 1.0f-2*blockSize;
    }
    
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }
    
    if (blockY < -1.0f+2*blockSize) {
        blockY = -1.0f+2*blockSize;
    }
    
    //移动矩形四个顶点坐标动态赋值
    vVerts[0] = blockX;
    vVerts[1] = blockY-2*blockSize;
    
    vVerts[3] = blockX+2*blockSize;
    vVerts[4] = blockY-2*blockSize;
    
    vVerts[6] = blockX+2*blockSize;
    vVerts[7] = blockY;
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    
    //提交数据
    squareBatch.CopyVertexData3f(vVerts);
    
    //窗口刷新
    glutPostRedisplay();
}

void SetupRC()
{
    //设置窗口背景颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //提交移动矩形顶点数据
    //GL_TRIANGLE_FAN：图元——三角形扇
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
    
    //设置并提交四个矩形框顶点数据
    GLfloat vBlock1[] = {//逆时针绘制——左下为第一个绘制顶点
        0.25f,0.25f,0.0f,
        0.75f,0.25,0.0f,
        0.75f,0.75f,0.0f,
        0.25f,0.75f,0.0f
    };
    redBatch.Begin(GL_TRIANGLE_FAN, 4);
    redBatch.CopyVertexData3f(vBlock1);
    redBatch.End();
    
    GLfloat vBlock2[] = {
        -0.75f,0.25f,0.0f,
        -0.25f,0.25f,0.0f,
        -0.25f,0.75f,0.0f,
        -0.75f,0.75f,0.0f
    };
    greenBatch.Begin(GL_TRIANGLE_FAN, 4);
    greenBatch.CopyVertexData3f(vBlock2);
    greenBatch.End();
    
    GLfloat vBlock3[] = {
        -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f
    };
    blueBatch.Begin(GL_TRIANGLE_FAN, 4);
    blueBatch.CopyVertexData3f(vBlock3);
    blueBatch.End();
    
    GLfloat vBlock4[] = {
        0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f
    };
    blackBatch.Begin(GL_TRIANGLE_FAN, 4);
    blackBatch.CopyVertexData3f(vBlock4);
    blackBatch.End();
    
}

int main(int argc,char *argv[])
{
    //色值工作目录
    gltSetWorkingDirectory(argv[0]);
    //初始化GLUT库
    glutInit(&argc, argv);
    //初始化显示模式：双缓冲、颜色缓冲、模版缓冲
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置窗口：大小、名字
    glutInitWindowSize(800, 600);
    glutCreateWindow("移动矩阵重叠——在上方");
    
    //自适应窗口大小
    glutReshapeFunc(ChangeSize);
    //方位键控制
    glutSpecialFunc(SpecialKeys);
    //图形渲染
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error:%s\n", glewGetErrorString(err));
        return 1;
    }
    
    //图形顶点处理
    SetupRC();
    
    //开启事件循环
    glutMainLoop();
    
    return  0;
    
}
