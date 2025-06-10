// RB_Dictionary.h
#pragma once

#include <vector>   // для NodePool
#include <stack>    // для clear()

template <typename Key, typename Value>
class RB_Dictionary {
private:
    enum Color : unsigned char { RED = 0, BLACK = 1 };

    struct Node {
        Key     key;
        Value   value;
        Node* left;
        Node* right;
        Node* parent;
        Color   color;

        // Конструктор: инициализируем узел как RED, а указатели временно — на nullptr
        Node(const Key& k, const Value& v)
            : key(k), value(v), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
        }
    };

    //--------------------------------------------------------------------------------------------
    //  Пул узлов: для ускоренного переиспользования памяти
    //  Вместо delete/new узел возвращается в пул при erase/clear, 
    //  и повторно используется при insert.
    //--------------------------------------------------------------------------------------------
    class NodePool {
        std::vector<Node*> pool;
    public:
        ~NodePool() {
            for (auto p : pool) {
                delete p;
            }
        }

        // Если есть узел в пуле, возвращаем его, иначе — nullptr
        inline Node* allocate_from_pool() {
            if (!pool.empty()) {
                Node* n = pool.back();
                pool.pop_back();
                return n;
            }
            return nullptr;
        }

        // Кладём узел в пул
        inline void deallocate(Node* node) {
            pool.push_back(node);
        }
    };

    Node* root;       // корень дерева
    Node* nil;        // единственный «sentinel» узел, вместо nullptr
    size_t      node_count; // число элементов
    NodePool    pool;       // пул узлов


    // Создаёт новый узел, либо берёт из пула и переинициализирует, либо выделяет через new
    inline Node* create_node(const Key& key, const Value& value) {
        Node* x = pool.allocate_from_pool();
        if (x) {
            x->key = key;
            x->value = value;
            x->left = nil;
            x->right = nil;
            x->parent = nil;
            x->color = RED;
            return x;
        }
        // В пуле ничего нет — вызываем new
        x = new Node(key, value);
        x->left = nil;
        x->right = nil;
        x->parent = nil;
        x->color = RED;
        return x;
    }

    // Освобождает узел — помещает его в пул (не вызывает delete)
    inline void destroy_node(Node* x) {
        pool.deallocate(x);
    }

    // Левый поворот вокруг узла x
    inline void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nil) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    // Правый поворот вокруг узла x
    inline void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nil) {
            root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    // Восстановление красно-чёрных свойств после вставки узла z
    inline void insertFixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right; // «дядя»
                if (y->color == RED) {
                    // Случай 1: дядя красный — перекрасим parent и uncle в чёрный, grandparent в красный
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else {
                    // Случай 2 или 3: дядя чёрный
                    if (z == z->parent->right) {
                        // Случай 2: «вогнутая» — левый поворот parent
                        z = z->parent;
                        leftRotate(z);
                    }
                    // Случай 3: «выпуклая» — перекрасим parent/grandparent и правый поворот grandparent
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            }
            else {
                // Симметричная логика для правой стороны
                Node* y = z->parent->parent->left;
                if (y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    // Пересадка поддерева u на место v (используется при удалении)
    inline void transplant(Node* u, Node* v) {
        if (u->parent == nil) {
            root = v;
        }
        else if (u == u->parent->left) {
            u->parent->left = v;
        }
        else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    // Поиск минимального узла в поддереве, начиная с node
    inline Node* minimum(Node* node) const {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    // Восстановление красно-чёрных свойств после удаления узла, начиная с узла x
    inline void deleteFixup(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) {
                    // Случай 1: брат w красный — перекрасим и левый поворот parent
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                // Случай 2: оба ребёнка брата чёрные — перекрасим брата и «поднимем» x
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->right->color == BLACK) {
                        // Случай 3: правый ребёнок брата чёрный, левый — красный
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // Случай 4: правый ребёнок брата красный
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else {
                // Симметричная логика для правой стороны
                Node* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                }
                else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

public:

    //  Конструктор: создаём единственный sentinel nil; весь «пустой» указатель указывает на nil.

    RB_Dictionary()
        : node_count(0)
    {
        // Создаём sentinel-узел nil (чёрный), потомки и родитель указывают на самого себя
        nil = new Node(Key(), Value());
        nil->color = BLACK;
        nil->left = nil->right = nil->parent = nil;
        root = nil;
    }


    ~RB_Dictionary() {
        clear();
        delete nil;
    }

    bool insert(const Key& key, const Value& value) {
        Node* y = nil;
        Node* x = root;

        // Спускаемся вниз, ищем место вставки
        while (x != nil) {
            y = x;
            if (key < x->key) {
                x = x->left;
            }
            else if (x->key < key) {
                x = x->right;
            }
            else {
                // ключ найден — обновляем значение
                x->value = value;
                return false;
            }
        }

        // x == nil, y — будущий родитель
        Node* z = create_node(key, value);
        z->parent = y;
        if (y == nil) {
            root = z;
        }
        else if (key < y->key) {
            y->left = z;
        }
        else {
            y->right = z;
        }
        // z->left и z->right уже указывают на nil

        insertFixup(z);
        ++node_count;
        return true;
    }


    Value& operator[](const Key& key) {
        Node* y = nil;
        Node* x = root;

        while (x != nil) {
            y = x;
            if (key < x->key) {
                x = x->left;
            }
            else if (x->key < key) {
                x = x->right;
            }
            else {
                // нашли, возвращаем ссылку на value
                return x->value;
            }
        }
        // x == nil → вставляем новый узел под y
        Node* z = create_node(key, Value());
        z->parent = y;
        if (y == nil) {
            root = z;
        }
        else if (key < y->key) {
            y->left = z;
        }
        else {
            y->right = z;
        }

        insertFixup(z);
        ++node_count;
        return z->value;
    }

    Value* find(const Key& key) const {
        Node* x = root;
        while (x != nil) {
            if (key < x->key) {
                x = x->left;
            }
            else if (x->key < key) {
                x = x->right;
            }
            else {
                return const_cast<Value*>(&x->value);
            }
        }
        return nullptr;
    }


    bool erase(const Key& key) {
        Node* z = root;
        // Ищем узел с ключом key
        while (z != nil) {
            if (key < z->key) {
                z = z->left;
            }
            else if (z->key < key) {
                z = z->right;
            }
            else {
                break;
            }
        }
        if (z == nil) {
            return false; // нет такого ключа
        }

        Node* y = z;
        Color y_original_color = y->color;
        Node* x;

        if (z->left == nil) {
            // Случай 1: нет левого ребёнка — просто «пересадить» правый вместо z
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == nil) {
            // Случай 2: нет правого ребёнка — «пересадить» левый вместо z
            x = z->left;
            transplant(z, z->left);
        }
        else {
            // Случай 3: оба потомка есть — найти преемника y (минимум в правом поддереве)
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;

            if (y->parent == z) {
                // Если преемник — непосредственный ребёнок z
                x->parent = y;
            }
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        // Помещаем старый узел z в пул
        destroy_node(z);
        --node_count;

        if (y_original_color == BLACK) {
            deleteFixup(x);
        }
        return true;
    }

    void clear() {
        if (root == nil) {
            return;
        }
        // Итеративный обход в глубину (LIFO), чтобы собрать все узлы
        std::vector<Node*> stack;
        stack.reserve(node_count);
        stack.push_back(root);

        while (!stack.empty()) {
            Node* curr = stack.back();
            stack.pop_back();
            if (curr->left != nil)  stack.push_back(curr->left);
            if (curr->right != nil) stack.push_back(curr->right);
            destroy_node(curr);
        }
        root = nil;
        node_count = 0;
    }

    inline size_t size() const {
        return node_count;
    }
};