#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

struct Node {
    string name;
    bool isFile;
    vector<unique_ptr<Node>> children;
    Node* parent;

    Node(const string& name, bool isFile, Node* parent = nullptr)
        : name(name), isFile(isFile), parent(parent) {}
};

class DirectoryTree {
public:
    DirectoryTree() {
        root = make_unique<Node>("root", false, nullptr);
    }

    bool addNode(const string& path, const string& name, bool isFile) {
        Node* parent = findNodeByPath(path);
        if (!parent) {
            cout << "Không tìm thấy đường dẫn: " << path << "\n";
            return false;
        }
        if (parent->isFile) {
            cout << "Không thể thêm vào một tệp tin. Chọn thư mục." << "\n";
            return false;
        }
        if (findChild(parent, name) != nullptr) {
            cout << "Tên đã tồn tại trong thư mục này: " << name << "\n";
            return false;
        }
        parent->children.push_back(make_unique<Node>(name, isFile, parent));
        return true;
    }

    bool deleteNode(const string& path) {
        if (path.empty() || path == "/" || path == "root") {
            cout << "Không thể xóa thư mục gốc." << "\n";
            return false;
        }
        string parentPath;
        string nodeName;
        if (!splitParentChild(path, parentPath, nodeName)) {
            cout << "Đường dẫn không hợp lệ: " << path << "\n";
            return false;
        }
        Node* parent = findNodeByPath(parentPath);
        if (!parent) {
            cout << "Không tìm thấy thư mục: " << parentPath << "\n";
            return false;
        }
        auto it = find_if(parent->children.begin(), parent->children.end(), [&](const unique_ptr<Node>& child) {
            return child->name == nodeName;
        });
        if (it == parent->children.end()) {
            cout << "Không tìm thấy mục cần xóa: " << nodeName << "\n";
            return false;
        }
        parent->children.erase(it);
        return true;
    }

    Node* findNodeByPath(const string& path) {
        if (path.empty() || path == "/" || path == "root") {
            return root.get();
        }
        vector<string> parts = splitPath(path);
        Node* current = root.get();
        for (const string& part : parts) {
            if (part.empty()) continue;
            current = findChild(current, part);
            if (!current) {
                return nullptr;
            }
        }
        return current;
    }

    void printTree() const {
        printSubtree(root.get(), 0);
    }

    void printNodePath(const string& path) {
        Node* node = findNodeByPath(path);
        if (!node) {
            cout << "Không tìm thấy đường dẫn: " << path << "\n";
            return;
        }
        cout << "Đường dẫn tìm thấy: " << buildPath(node) << "\n";
        cout << "Loại: " << (node->isFile ? "Tệp tin" : "Thư mục") << "\n";
    }

private:
    unique_ptr<Node> root;

    static vector<string> splitPath(const string& path) {
        vector<string> result;
        string current;
        stringstream ss(path);
        while (getline(ss, current, '/')) {
            if (!current.empty()) {
                result.push_back(current);
            }
        }
        return result;
    }

    static bool splitParentChild(const string& path, string& parentPath, string& childName) {
        vector<string> parts = splitPath(path);
        if (parts.empty()) {
            return false;
        }
        childName = parts.back();
        if (parts.size() == 1) {
            parentPath = "root";
        } else {
            parentPath.clear();
            for (size_t i = 0; i + 1 < parts.size(); ++i) {
                parentPath += parts[i];
                if (i + 1 < parts.size() - 1) {
                    parentPath += "/";
                }
            }
        }
        return true;
    }

    static Node* findChild(Node* node, const string& name) {
        for (const auto& child : node->children) {
            if (child->name == name) {
                return child.get();
            }
        }
        return nullptr;
    }

    static string buildPath(Node* node) {
        vector<string> parts;
        while (node) {
            parts.push_back(node->name);
            node = node->parent;
        }
        reverse(parts.begin(), parts.end());
        string path;
        for (size_t i = 0; i < parts.size(); ++i) {
            path += parts[i];
            if (i + 1 < parts.size()) {
                path += "/";
            }
        }
        return path;
    }

    void printSubtree(const Node* node, int depth) const {
        string indent(depth * 2, ' ');
        cout << indent << (node == root.get() ? "[root]" : node->name)
             << (node->isFile ? " (file)" : " (folder)") << "\n";
        for (const auto& child : node->children) {
            printSubtree(child.get(), depth + 1);
        }
    }
};

void showMenu() {
    cout << "\n==== Mô phỏng cây thư mục (General Tree) ====\n";
    cout << "1. Thêm thư mục" << "\n";
    cout << "2. Thêm tệp tin" << "\n";
    cout << "3. Xóa mục" << "\n";
    cout << "4. Hiển thị cây thư mục" << "\n";
    cout << "5. Tìm đường dẫn" << "\n";
    cout << "6. Thoát" << "\n";
    cout << "Chọn một tùy chọn: ";
}

int main() {
    DirectoryTree tree;
    int choice = 0;
    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Lựa chọn không hợp lệ. Vui lòng thử lại." << "\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1 || choice == 2) {
            string path, name;
            cout << "Nhập đường dẫn thư mục cha (vd: root/folder1): ";
            getline(cin, path);
            cout << "Nhập tên " << (choice == 1 ? "thư mục" : "tệp tin") << ": ";
            getline(cin, name);
            if (name.empty()) {
                cout << "Tên không được để trống." << "\n";
                continue;
            }
            bool success = tree.addNode(path, name, choice == 2);
            if (success) {
                cout << "Đã thêm thành công." << "\n";
            }
        } else if (choice == 3) {
            string path;
            cout << "Nhập đường dẫn mục cần xóa (vd: root/folder1/file.txt): ";
            getline(cin, path);
            if (tree.deleteNode(path)) {
                cout << "Xóa thành công." << "\n";
            }
        } else if (choice == 4) {
            tree.printTree();
        } else if (choice == 5) {
            string path;
            cout << "Nhập đường dẫn cần tìm: ";
            getline(cin, path);
            tree.printNodePath(path);
        } else if (choice == 6) {
            cout << "Thoát chương trình." << "\n";
            break;
        } else {
            cout << "Lựa chọn không hợp lệ. Vui lòng nhập số từ 1 đến 6." << "\n";
        }
    }
    return 0;
}
