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

bool readInt(const char* S, int& idx, int& num) {
    num = 0;
    while (S[idx] == ' ') idx++;
    bool neg = false;
    if (S[idx] == '-') {
        neg = true;
        idx++;
    }
    else if (S[idx] == '+') {
        idx++;
    }
    while (isdigit(S[idx])) {
        num *= 10;
        num += S[idx] - '0';
        idx++;
    }
    if (neg) num *= -1;
    return true;
}


std::vector<Vertex> ObjLoader::getVertices() const {
    std::vector<Vertex> vertices;
    Vertex v[3];
    for (auto f : Triangles) {
        memset(v, 0, sizeof(v));
        for (int i = 0; i < 3; i++) {
            if (f.v[i])
                v[i].pos = glm::vec4(Vertices[f.v[i] - 1], 1.0f);
            if (f.vt[i])
                v[i].texCoord = TexCoords[f.vt[i] - 1];
            if (f.vn[i])
                v[i].normal = Normals[f.vn[i] - 1];
            v[i].color = glm::vec3(1);
            vertices.push_back(v[i]);
        }

    }
    return vertices;
}

void ObjLoader::process() {
    static char start[16];
    static char faceV[105];
    sscanf(lineBuffer + _ptr, "%s", start);
    _ptr += strlen(start);

    if (!strcmp(start, "v")) {
        double x, y, z;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf%lf", &x, &y, &z);
        Vertices.push_back(glm::vec3(x, y, z));
    }
    else if (!strcmp(start, "f")) {
        int c = 0;
        std::vector<std::tuple<int, int, int>> vertices;
        while (~(c = sscanf(lineBuffer + _ptr, "%s", faceV))) {
            int id = 0;
            int vd[3];
            memset(vd, 0, sizeof(vd));
            for (int j = 0; j < 3; j++) {
                bool b = readInt(faceV, id, vd[j]);
                id++;
                if (!b)break;
            }
            vertices.push_back({ vd[0], vd[1], vd[2] });
            //int i = 0;
            //int num = 0;
            //bool neg = false;
            //for (; faceV[i] != '\0' && faceV[i] != '/'; i++) {
            //    if (!isdigit(faceV[i])) {
            //        if (faceV[i] == '-') {
            //            neg = true;
            //        }
            //        continue;
            //    }
            //    // assert(isdigit(faceV[i]));
            //    num *= 10;
            //    num += faceV[i] - '0';
            //}
            //if (!neg)
            //    num--;
            //else
            //    num = Vertices.size() - num - 1;
            //vertices.push_back(num);
            _ptr += strlen(faceV) + 1;
        }

        // Triangulation process
        auto cmp = [&](int a, int b) {
            return Vertices[a].x < Vertices[b].x;
        };
        int sz = vertices.size();
        if (sz == 3) {
            Triangles.push_back(Face(vertices));
        }
        else {
            std::cerr << "Invalid obj file format" << std::endl;
            assert(false);
            return;
        }
    }
    else if (!strcmp(start, "vt")) {
        double x, y;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf", &x, &y);
        TexCoords.push_back(glm::vec2(x, y));
    }
    else if (!strcmp(start, "vn")) {
        double x, y, z;
        _ptr += sscanf(lineBuffer + _ptr, "%lf%lf%lf", &x, &y, &z);
        Normals.push_back(glm::vec3(x, y, z));
    }
}
