#include "Main.h"
#include <glm/gtx/transform.hpp>
#include <vector>
#include "OBJ_Loader.h"


glm::mat4 projection_matrix(float zNear, float zFar, float fov, float aspectRatio) {
    float h = tan(fov / 2) * zNear;
    float w = aspectRatio * h;
    return glm::mat4(zNear / w, 0, 0, 0,
        0, zNear / h, 0, 0,
        0, 0, (zNear + zFar) / (zNear - zFar), -1,
        0, 0, 2 * zNear * zFar / (zNear - zFar), 0);
}

Main::Main(HWND hwnd, int width, int height) :hWnd(hwnd), _width(width), _height(height) {
    _rasterizer = new Rasterizer(width, height, SampleStyle::None);
    objl::Loader _loader;
    _loader.LoadFile("models/cube.obj");
    for (auto mesh : _loader.LoadedMeshes) {
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            Triangle t;
            memset(&t, 0, sizeof(t));
            for (int j = 0; j < 3; j++) {
                t.color[j] = glm::vec3(1, 1, 1);
                t.vs[j] = glm::vec4(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.f);
                t.normals[j] = glm::vec3(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z);
                t.tex_coords[j] = glm::vec2(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y);;
            }
            _triangles.push_back(t);
        }
    }
    _camera.SetPosition(glm::vec3(0, 5, 10));
}

void Main::Run() {
    _rasterizer->Clear();
    static float rot = 0;
    Model m1(_triangles);
    m1.SetTransform(glm::rotate(rot, glm::vec3(0, 0, 1)));
    // rot += 0.1f;
    Model m2(_triangles);
    m2.SetTransform(glm::translate(glm::vec3(0, -4, 0)) * glm::scale(glm::vec3(5, 1, 5)));
    m2.SetColor(glm::vec3(1, 0, 0));
    Model m3(_triangles);
    m3.SetTransform(glm::translate(glm::vec3(0, 5, -5)) * glm::scale(glm::vec3(0.3, 0.3, 0.3)));
    m3._shader = [](const fragment_shader_payload& info) -> glm::vec3 {
        return glm::vec3(1, 1, 1);
    };
    std::vector<Model> models;
    models.push_back(m1);
    models.push_back(m2);

    _rasterizer->SetCamera(&_camera);
    _rasterizer->SetProjectionMatrix(projection_matrix(0.1f, 50, glm::pi<float>() / 4, 4.f / 3.f));
    _rasterizer->CreateShadowMap(models);

    models.push_back(m3);
    _rasterizer->DrawModels(models, DrawStyle::Fill);

    _rasterizer->Present();

    HDC hdc = GetDC(hWnd);
    BITMAPINFO info;
    ZeroMemory(&info, sizeof(BITMAPINFO));
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biWidth = 800;
    info.bmiHeader.biHeight = 600;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biSizeImage = 800 * 600;
    info.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, 800, 600, 0, 0, 800, 600, _rasterizer->getFrameBuffer(), &info, DIB_RGB_COLORS, SRCCOPY);

    //auto file = fopen("test.bmp", "w");
    //BITMAPFILEHEADER bmpHeader;
    //bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 800 * 600;  // BMP图像文件的大小
    //bmpHeader.bfType = 0x4D42;  // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
    //bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);          // BMP图像数据的偏移位置
    //bmpHeader.bfReserved1 = 0;  // 总为0
    //bmpHeader.bfReserved2 = 0;  // 总为0
    //DWORD dwBytesWritten = 0;
    //fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, file);
    //BITMAPINFOHEADER bmiHeader;
    //bmiHeader.biSize = sizeof(bmiHeader);               // 本结构所占用字节数，即sizeof(BITMAPINFOHEADER);
    //bmiHeader.biWidth = 800;                          // 位图宽度（单位：像素）
    //bmiHeader.biHeight = 600;                        // 位图高度（单位：像素）
    //bmiHeader.biPlanes = 1;                             // 目标设备的级别，必须为1
    //bmiHeader.biBitCount = 24;                    // 像素的位数（每个像素所需的位数，范围：1、4、8、24、32）
    //bmiHeader.biCompression = 0;                        // 压缩类型（0：不压缩 1：BI_RLE8压缩类型 2：BI_RLE4压缩类型）
    //bmiHeader.biSizeImage = 800 * 600;                   // 位图大小（单位：字节）
    //bmiHeader.biXPelsPerMeter = 0;                      // 水平分辨率(像素/米)
    //bmiHeader.biYPelsPerMeter = 0;                      // 垂直分辨率(像素/米)
    //bmiHeader.biClrUsed = 0;                            // 位图实际使用的彩色表中的颜色索引数
    //bmiHeader.biClrImportant = 0;                       // 对图象显示有重要影响的颜色索引的数目
    //// 【写位图信息头（BITMAPINFO的bmiHeader成员）】
    //fwrite(&bmiHeader, sizeof(BITMAPINFOHEADER), 1, file);

    //fwrite(_rasterizer->getFrameBuffer(), sizeof(RColor) * 800 * 600, 1, file);
    //fclose(file);

    ReleaseDC(hWnd, hdc);
}
