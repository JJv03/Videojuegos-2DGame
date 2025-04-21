#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <SFML/Graphics.hpp>


template <typename T>
class BVHTree {
public:
    struct Node {
        sf::FloatRect bounds;
        Node* left = nullptr;
        Node* right = nullptr;
        std::vector<T*> objects;

        bool isLeaf() const { return left == nullptr && right == nullptr; }
    };

    BVHTree() : root(nullptr) {}
    ~BVHTree() { clear(); }

    void build(const std::vector<T*>& objects) {
        root = buildRecursive(objects, 0);
    }

    template <typename Func>
    void queryAndCollide(T& target, Func&& callback) const {
        queryAndCollideRecursive(root, target, callback);
    }

    void clear() {
        destroyRecursive(root);
        root = nullptr;
    }

private:
    Node* root;

    Node* buildRecursive(std::vector<T*> objects, int depth) {
        if (objects.empty()) return nullptr;

        Node* node = new Node();
        if (objects.size() <= 2) {
            node->objects = std::move(objects);
            node->bounds = computeBounds(node->objects);
            return node;
        }

        bool sortByX = (depth % 2 == 0);
        std::sort(objects.begin(), objects.end(), [&](T* a, T* b) {
            const auto rectA = getEnclosingBounds(a->getBounds());
            const auto rectB = getEnclosingBounds(b->getBounds());
            return sortByX ? rectA.position.x < rectB.position.x : rectA.position.y < rectB.position.y;
        });

        size_t mid = objects.size() / 2;
        std::vector<T*> left(objects.begin(), objects.begin() + mid);
        std::vector<T*> right(objects.begin() + mid, objects.end());

        node->left = buildRecursive(left, depth + 1);
        node->right = buildRecursive(right, depth + 1);
        node->bounds = unite(node->left->bounds, node->right->bounds);
        return node;
    }

    void destroyRecursive(Node* node) {
        if (!node) return;
        destroyRecursive(node->left);
        destroyRecursive(node->right);
        delete node;
    }

    sf::FloatRect unite(const sf::FloatRect& a, const sf::FloatRect& b) const {
        float left = std::min(a.position.x, b.position.x);
        float top = std::min(a.position.y, b.position.y);
        float right = std::max(a.position.x + a.size.x, b.position.x + b.size.x);
        float bottom = std::max(a.position.y + a.size.y, b.position.y + b.size.y);
        return sf::FloatRect({left, top}, {right - left, bottom - top});
    }

    sf::FloatRect computeBounds(const std::vector<T*>& objs) const {
        sf::FloatRect result = getEnclosingBounds(objs[0]->getBounds());
        for (size_t i = 1; i < objs.size(); ++i)
            result = unite(result, getEnclosingBounds(objs[i]->getBounds()));
        return result;
    }

    sf::FloatRect getEnclosingBounds(const std::vector<sf::FloatRect>& rects) const {
        sf::FloatRect result = rects[0];
        for (size_t i = 1; i < rects.size(); ++i)
            result = unite(result, rects[i]);
        return result;
    }

    template <typename Func>
    void queryAndCollideRecursive(Node* node, T& target, Func&& callback) const {
        if (target.getBounds().empty()) return;

        if (!node || !checkIntersection(node->bounds, target.getBounds())) return;

        if (node->isLeaf()) {
            for (T* obj : node->objects) {
                if (obj == &target) continue;
                if (checkIntersection(obj->getBounds(), target.getBounds())) {
                    callback(*obj);
                }
            }
        } else {
            queryAndCollideRecursive(node->left, target, callback);
            queryAndCollideRecursive(node->right, target, callback);
        }
    }

    bool checkIntersection(sf::FloatRect a, const std::vector<sf::FloatRect>& b) const {
        for (const auto& rect : b) {
            if (areIntersecting(a, rect)) {
                return true;
            }
        }
        return false;   // No intersection found
    }

    bool checkIntersection(const std::vector<sf::FloatRect>& a, const std::vector<sf::FloatRect>& b) const {
        for (const auto& rect_A : a) {
            for (const auto& rect_B : b) {
                if (areIntersecting(rect_A, rect_B)) {
                    return true;
                }
            }
        }
        return false;   // No intersection found
    }

    bool areIntersecting(const sf::FloatRect& a, const sf::FloatRect& b) const {
        return !(a.position.x + a.size.x < b.position.x ||
                a.position.x > b.position.x + b.size.x ||
                a.position.y + a.size.y < b.position.y ||
                a.position.y > b.position.y + b.size.y);
    }
};
