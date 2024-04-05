#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdbool.h> 

typedef struct Node {
    int key;
    int val;
    int degree;
    struct Node* parent;
    struct Node* child;
    struct Node* left_sibling;
    struct Node* right_sibling;
    bool child_cut_flag;
} Node;

Node* createNode(int key, int val) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->val = val;
    node->degree = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left_sibling = node;
    node->right_sibling = node;
    node->child_cut_flag = false;
    return node;
}

Node* insertNode(Node* heap, int key, int val) {
    Node* node = createNode(key, val); //根據key/val創建新Node
    if (heap == NULL) { //若第一次建立，則直接指定新Node為Heap
        return node;
    }

    Node* ptr = heap->left_sibling;
    node->left_sibling = ptr;
    node->right_sibling = heap;
    ptr->right_sibling = node;
    heap->left_sibling = node;

    if (node->key < heap->key) {
        heap = node;
    }
    return heap;
}

Node* unionHeaps(Node* heap1, Node* heap2) {
    if (heap1 == NULL) {
        return heap2;
    }
    if (heap2 == NULL) {
        return heap1;
    }
    if (heap1 == heap2) {
        return heap1;
    }

    if (heap1->right_sibling == heap2 && heap1->left_sibling == heap2) {
        heap1->left_sibling = heap1;
        heap1->right_sibling = heap1;
    }

    if (heap2->left_sibling == heap1 && heap2->right_sibling == heap1) {
        heap2->left_sibling = heap2;
        heap2->right_sibling = heap2;
    }

    Node* heap1Last = heap1->left_sibling;
    Node* heap2Last = heap2->left_sibling;

    heap1Last->right_sibling = heap2;
    heap2->left_sibling = heap1Last;

    heap2Last->right_sibling = heap1;
    heap1->left_sibling = heap2Last;
    
    Node* newHeap = (heap1->key < heap2->key) ? heap1 : heap2;
    return newHeap;
}

void fibHeapLink(Node* node1, Node* node2) {
    if (node1 == node2 || node1 == NULL || node2 == NULL) { // 避免將節點連接到自己或空節點
        return;
    }

    node2->left_sibling->right_sibling = node2->right_sibling;
    node2->right_sibling->left_sibling = node2->left_sibling;

    node2->parent = node1;
    node2->left_sibling = node2;
    node2->right_sibling = node2;
    node2->child_cut_flag = false;

    if (node1->child == NULL) {
        node1->child = node2;
    } else {
        Node* ptr = node1->child;
        node2->right_sibling = ptr->right_sibling;
        node2->left_sibling = ptr;
        ptr->right_sibling->left_sibling = node2;
        ptr->right_sibling = node2;

        if (node2->key < ptr->key) {
            node1->child = node2;
        }
    }

    node1->degree++;
}

void consolidateRoots(Node** heap) {
    if (*heap == NULL) {
        return;
    }

    int n = 100000; // 題目操作上限為10的5次方
    int maxDegree = (int)floor(log2(n)) + 1; // 使用操作數上限來估計最大度數
    Node** arr = (Node**)malloc(maxDegree * sizeof(Node*));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < maxDegree; i++) {
        arr[i] = NULL;
    }

    Node* ptr = *heap;
    do {
        Node* next = ptr->right_sibling;
        int degree = ptr->degree;

        while (degree < maxDegree && arr[degree] != NULL) {
            Node* other = arr[degree];
            if (other == ptr) {
                break;
            }

            if (ptr->key > other->key) { //若ptr比other大，則交換位置
                Node* temp = ptr;
                ptr = other;
                other = temp;
            }

            fibHeapLink(ptr, other); //將other放在ptr底下
            arr[degree] = NULL;
            degree++;
        }

        if (degree < maxDegree) {
            arr[degree] = ptr;
        }

        ptr = next;
    } while (ptr != *heap); // 修改為使用 *heap 作為終止條件

    *heap = NULL;
    for (int i = 0; i < maxDegree; i++) {
        if (arr[i] != NULL) {
            if (*heap == NULL) {
                *heap = arr[i];
                arr[i]->left_sibling = arr[i]->right_sibling = arr[i]; //將同heap同一層的sibling串聯
            } else {
                *heap = unionHeaps(*heap, arr[i]);
            }
        }
    }

    free(arr);
}

void addChildrenToRootList(Node* heap, Node* node) {
    Node* child = node->child;
    if (child != NULL) {
        Node* temp = child;
        Node* firstChild = child;
        Node* lastChild = child->left_sibling;

        do {
            temp->parent = NULL; // 斷開父子關係
            temp = temp->right_sibling;
        } while (temp != child);

        // 將節點的子節點列表連接到根列表中
        Node* leftSib = heap->left_sibling;
        Node* rightSib = heap;

        leftSib->right_sibling = firstChild;
        firstChild->left_sibling = leftSib;
        rightSib->left_sibling = lastChild;
        lastChild->right_sibling = rightSib;
    }
    node->child = NULL; // 將節點的 child 設置為 NULL
}

Node* extractMinNode(Node** heap) {
    if (*heap == NULL) {
        return NULL;
    }

    Node* minNode = *heap;

    addChildrenToRootList(*heap, minNode);

    if (minNode == minNode->right_sibling) {
        *heap = NULL;
    } else {
        minNode->left_sibling->right_sibling = minNode->right_sibling;
        minNode->right_sibling->left_sibling = minNode->left_sibling;
        *heap = minNode->right_sibling;

        minNode->left_sibling = NULL;
        minNode->right_sibling = NULL;
    }

    if (*heap != NULL) {
        consolidateRoots(heap);
    }
    return minNode;
}

Node* find(Node* heap, int key, int val) {
    if (heap == NULL) {
        return NULL;
    }

    Node* ptr = heap;
    Node* result = NULL;

    do {
        if (ptr->key == key && ptr->val == val) {
            result = ptr;
            break;
        }

        if (ptr->child != NULL) {
            result = find(ptr->child, key, val);
            if (result != NULL) {
                break;
            }
        }

        ptr = ptr->right_sibling;
    } while (ptr != heap);

    return result;
}

void cut(Node* heap, Node* node, Node* parent) {
    if (parent->child == node) {
        if (node->right_sibling == node) {
            parent->child = NULL; // 如果節點是父節點的唯一子節點,將父節點的 child 設置為 NULL
        } else {
            parent->child = node->right_sibling;
        }
    }

    node->right_sibling->left_sibling = node->left_sibling;
    node->left_sibling->right_sibling = node->right_sibling;
    parent->degree--;

    addChildrenToRootList(heap, node);

    node->left_sibling = heap;
    node->right_sibling = heap->right_sibling;
    heap->right_sibling = node;
    node->right_sibling->left_sibling = node;
    node->parent = NULL;
    node->child = NULL; // 將節點的 child 設置為 NULL
    node->child_cut_flag = false;
}

void cascadingCut(Node* heap, Node* node) {
    Node* parent = node->parent;
    while (parent != NULL) {
        if (!node->child_cut_flag) {
            node->child_cut_flag = true;
            break;
        } else {
            cut(heap, node, parent);
            node = parent;
            parent = node->parent;
        }
    }
}

void decreaseKey(Node** heap, int key, int val, int decreaseAmount) {
    Node* node = find(*heap, key, val);
    if (node == NULL) {
        return;
    }
    
    int newKey = node->key - decreaseAmount; // 計算新的鍵值
    
    node->key = newKey; // 更新節點的鍵值
    Node* parent = node->parent;

    if (parent != NULL && node->key < parent->key) { // 執行剪切操作
        cut(*heap, node, parent);
        cascadingCut(*heap, parent);
    }
    
    if (newKey < (*heap)->key) { // 更新堆的最小節點指針
        *heap = node;
    }
}

void deleteKey(Node** heap, int key, int val) {
    Node* node = find(*heap, key, val);
    if (node == NULL) {
        return;
    }

    decreaseKey(heap, node->key, node->val, INT_MAX);
    Node* minNode = extractMinNode(heap);
    if (minNode != node) {

    }
    free(minNode);
}

int main(int argc, char* argv[]) {
    char operation[20];
    int arg1, arg2;
    Node* heap = NULL;

    while (scanf("%s", operation) != EOF) {
        if (strcmp(operation, "insert") == 0) {
            scanf("%d %d", &arg1, &arg2);
            heap = insertNode(heap, arg1, arg2);
        } else if (strcmp(operation, "delete") == 0) {
            scanf("%d %d", &arg1, &arg2);
            deleteKey(&heap, arg1, arg2);
        } else if (strcmp(operation, "decrease") == 0) {
            int newKey;
            scanf("%d %d %d", &arg1, &arg2, &newKey);
            decreaseKey(&heap, arg1, arg2, newKey);
        } else if (strcmp(operation, "extract") == 0) {
            Node* minNode = extractMinNode(&heap);
            if (minNode != NULL) {
                printf("(%d)%d\n", minNode->key, minNode->val); // stdout
                free(minNode);
            }
        } else if (strcmp(operation, "quit") == 0) {
            break;
        }
    }

    return 0;
}