#include "ObjLoader.h"
char ObjLoader::lineBuffer[MAX_BUFFER];

void ObjLoader::load(const std::string& path) {
    Vertices.clear();
    Triangles.clear();
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        std::cerr << "Cannot open file " << path << std::endl;
        return;
    }
    while (fgets(lineBuffer, MAX_BUFFER, file)) {
        _ptr = 0;
        process();
    }
}

void ObjLoader::process() {
    static char start[16];
    static char faceV[105];
    _ptr += sscanf(lineBuffer + _ptr, "%s", start);

    Vertex vertex;
    memset(&vertex, 0, sizeof(vertex));
    if (!strcmp(start, "v")) {
        double x, y, z;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf%lf", &x, &y, &z);
        vertex.pos = glm::vec4(x, y, z, 1.0f);
        vertex.color = glm::vec3(1);
        Vertices.push_back(vertex);
    }
    else if (!strcmp(start, "f")) {
        int c = 0;
        std::vector<int> vertices;
        while (~(c = sscanf(lineBuffer + _ptr, "%s", faceV))) {
            int i = 0;
            int num = 0;
            bool neg = false;
            for (; faceV[i] != '\0' && faceV[i] != '/'; i++) {
                if (!isdigit(faceV[i])) {
                    if (faceV[i] == '-') {
                        neg = true;
                    }
                    continue;
                }
                // assert(isdigit(faceV[i]));
                num *= 10;
                num += faceV[i] - '0';
            }
            if (!neg)
                num--;
            else
                num = Vertices.size() - num - 1;
            vertices.push_back(num);
            _ptr += strlen(faceV) + c;
        }

        // Triangulation process
        auto cmp = [&](int a, int b) {
            return Vertices[a].pos.x < Vertices[b].pos.x;
        };
        std::sort(vertices.begin(), vertices.end());
        int sz = vertices.size();
        if (sz == 3) {
            Triangles.push_back(Face(vertices[0], vertices[1], vertices[2]));
        }
        else {
            std::cerr << "Invalid obj file format" << std::endl;
            assert(false);
            return;
        }
    }
}
