#pragma once
#include <iostream>
#include <vector>
#include "DataProject.cpp"
#include <algorithm>
#include "BigInt.h"
#include <fstream>
using namespace std;
struct Pair
{
    BigInt key;
    string value;
};
struct BTreeNode {
    bool leaf;
    std::vector<BigInt> keys;
    //vector<string> Hex;
    std::vector<std::string> values;  // Added string values
    std::vector<BTreeNode*> children;

    BTreeNode(bool isLeaf) : leaf(isLeaf) {}
};

class BTree {
private:
    BTreeNode* root;
    int minDegree;

public:
    BTree(int t) : root(nullptr), minDegree(t) {}


    // Helper function to merge a child with its right sibling
    void mergeChildren(BTreeNode* node, int childIndex) {
        BTreeNode* child = node->children[childIndex];
        BTreeNode* rightSibling = node->children[childIndex + 1];

        // Append the parent's key and value to the end of the left child's keys and values
        child->keys.push_back(node->keys[childIndex]);
        child->values.push_back(node->values[childIndex]);

        // Append all keys, values, and children from the right sibling to the left child
        child->keys.insert(child->keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
        child->values.insert(child->values.end(), rightSibling->values.begin(), rightSibling->values.end());
        child->children.insert(child->children.end(), rightSibling->children.begin(), rightSibling->children.end());

        // Remove the key and value from the parent
        node->keys.erase(node->keys.begin() + childIndex);
        node->values.erase(node->values.begin() + childIndex);

        // Remove the right sibling
        delete rightSibling;
        node->children.erase(node->children.begin() + childIndex + 1);
    }


    // Helper function to find the predecessor of a key
    BigInt findPredecessor(BTreeNode* node) {
        while (!node->leaf) {
            node = node->children[node->children.size() - 1];
        }
        return node->keys[node->keys.size() - 1];
    }

    // Helper function to find the successor of a key
    BigInt findSuccessor(BTreeNode* node) {
        while (!node->leaf) {
            node = node->children[0];
        }
        return node->keys[0];
    }


    // Helper function to fill the child with at least t keys
    void fillChild(BTreeNode* node, int childIndex) {
        // Borrow a key from the left sibling if possible
        if (childIndex > 0 && node->children[childIndex - 1]->keys.size() >= minDegree) {
            BTreeNode* child = node->children[childIndex];
            BTreeNode* leftSibling = node->children[childIndex - 1];

            // Shift all keys in child to the right
            child->keys.insert(child->keys.begin(), node->keys[childIndex - 1]);
            child->values.insert(child->values.begin(), node->values[childIndex - 1]);

            // Move the rightmost key and value from the left sibling to the parent
            node->keys[childIndex - 1] = leftSibling->keys[leftSibling->keys.size() - 1];
            node->values[childIndex - 1] = leftSibling->values[leftSibling->values.size() - 1];

            // If the child is not a leaf, move the child pointer from the left sibling
            if (!child->leaf) {
                child->children.insert(child->children.begin(), leftSibling->children[leftSibling->children.size() - 1]);
                leftSibling->children.pop_back();
            }

            // Remove the rightmost key and value from the left sibling
            leftSibling->keys.pop_back();
            leftSibling->values.pop_back();
        }
        // Borrow a key from the right sibling if possible
        else if (childIndex < node->keys.size() && node->children[childIndex + 1]->keys.size() >= minDegree) {
            BTreeNode* child = node->children[childIndex];
            BTreeNode* rightSibling = node->children[childIndex + 1];

            // Append the parent's key and value to the end of the child's keys and values
            child->keys.push_back(node->keys[childIndex]);
            child->values.push_back(node->values[childIndex]);

            // Move the leftmost key and value from the right sibling to the parent
            node->keys[childIndex] = rightSibling->keys[0];
            node->values[childIndex] = rightSibling->values[0];

            // If the child is not a leaf, move the child pointer from the right sibling
            if (!child->leaf) {
                child->children.push_back(rightSibling->children[0]);
                rightSibling->children.erase(rightSibling->children.begin());
            }

            // Remove the leftmost key and value from the right sibling
            rightSibling->keys.erase(rightSibling->keys.begin());
            rightSibling->values.erase(rightSibling->values.begin());
        }
        // Merge with the left sibling
        else if (childIndex > 0) {
            mergeChildren(node, childIndex - 1);
        }
        // Merge with the right sibling
        else {
            mergeChildren(node, childIndex);
        }
    }
    string findSuccessor(BTreeNode* node, int a) {
        while (!node->leaf) {
            node = node->children[0];
        }
        return node->values[0];
    }
    string findPredecessor(BTreeNode* node,int a) {
        while (!node->leaf) {
            node = node->children[node->children.size() - 1];
        }
        return node->values[node->values.size() - 1];
    }
    // Helper function to remove a key from a non-leaf node
    void removeFromNonLeaf(BTreeNode* node, const BigInt& key, const std::string& value) {
        int idx = 0;
        while (idx < node->keys.size() && ((key >= node->keys[idx]) && value!=(node->values[idx]))) {
            idx++;
        }

        if (idx < node->keys.size() && key == node->keys[idx] && value == node->values[idx]) {
            if (node->children[idx]->keys.size() >= minDegree) {
                BigInt pred = findPredecessor(node->children[idx]);
                node->keys[idx] = pred;
                node->values[idx] = findPredecessor(node->children[idx], 1);
                deleteKey(node->children[idx], pred, node->values[idx]);
            }
            else if (node->children[idx + 1]->keys.size() >= minDegree) {
                BigInt succ = findSuccessor(node->children[idx + 1]);
                node->keys[idx] = succ;
                node->values[idx] = findSuccessor(node->children[idx + 1], 1);
                deleteKey(node->children[idx + 1], succ, node->values[idx]);
            }
            else {
                mergeChildren(node, idx);
                deleteKey(node->children[idx], key, value);
            }
        }
        else {
            if (node->children[idx]->keys.size() == minDegree - 1) {
                fillChild(node, idx);
            }
            deleteKey(node->children[idx], key, value);
        }
    }

    void deleteKey(BTreeNode* node, const BigInt& key, const std::string& value) {
        if (node->leaf) {
            bool found = false;
            int index=0;
            for (int i = 0; i < node->values.size(); i++)
            {
                if (key == node->keys[i] && value == node->values[i])
                {
                    found = true;
                    break;
                }
                index++;
            }
            if (found == true)
            {
                if (node->values.size() > 1)
                {
                    swap(node->values[index], node->values.back());
                    swap(node->keys[index], node->keys.back());
                }
                 node->values.pop_back();
                 node->keys.pop_back();
            }
        }
        else {
            removeFromNonLeaf(node, key, value);
        }
    }

    void deleteKey(const BigInt& key, const std::string& value) {
        if (root == nullptr) {
            std::cout << "The tree is empty." << std::endl;
            return;
        }

        deleteKey(root, key, value);

        if (root->keys.empty() && !root->leaf) {
            BTreeNode* newRoot = root->children[0];
            delete root;
            root = newRoot;
        }
    }



    bool search(BTreeNode* node, const BigInt& key) {
        if (node != nullptr) {
            int i = 0;
            while (i < node->keys.size() && key > node->keys[i]) {
                i++;
            }

            if (i < node->keys.size() && key == node->keys[i]) {
                return true;
            }

            if (node->leaf) {
                return false;
            }

            return search(node->children[i], key);
        }

        return false;
    }
    /*pair<string, pair<string, BigInt>>* searchNode(BTreeNode* node, const BigInt& key) {
        if (node != nullptr) {
            int i = 0;
            while (i < node->keys.size() && key > node->keys[i]) {
                i++;
            }

            if (i < node->keys.size() && key == node->keys[i]) {
                pair<string, pair<string, BigInt>> ret = { node->values[i], {node->Hex[i], node->keys[i]} };
                return &ret;
            }

            if (node->leaf) {
                return nullptr;
            }

            return searchNode(node->children[i], key);
        }

        return nullptr;
    }*/

    void insert(const BigInt& key, const std::string& value) {
        if (root == nullptr) {
            root = new BTreeNode(true);
            root->keys.push_back(key);
            root->values.push_back(value);
        }
        else {
            if (root->keys.size() == (2 * minDegree - 1)) {
                BTreeNode* newRoot = new BTreeNode(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0);
                root = newRoot;
            }

            insertNonFull(root, key, value);
        }
    }

    void insertNonFull(BTreeNode* node, const BigInt& key, const std::string& value) {
        int i = node->keys.size() - 1;

        if (node->leaf) {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;

            node->keys.insert(node->keys.begin() + i, key);
            node->values.insert(node->values.begin() + i, value);
        }
        else {
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;

            if (node->children[i]->keys.size() == (2 * minDegree - 1)) {
                splitChild(node, i);

                if (key > node->keys[i]) {
                    i++;
                }
            }

            insertNonFull(node->children[i], key, value);
        }
    }

    void splitChild(BTreeNode* parent, int index) {
        BTreeNode* oldChild = parent->children[index];
        BTreeNode* newChild = new BTreeNode(oldChild->leaf);

        newChild->keys.insert(newChild->keys.begin(), oldChild->keys.begin() + minDegree, oldChild->keys.end());
        newChild->values.insert(newChild->values.begin(), oldChild->values.begin() + minDegree, oldChild->values.end());

        oldChild->keys.resize(minDegree);
        oldChild->values.resize(minDegree);

        if (!oldChild->leaf) {
            newChild->children.insert(newChild->children.begin(), oldChild->children.begin() + minDegree, oldChild->children.end());
            oldChild->children.resize(minDegree);
        }

        parent->keys.insert(parent->keys.begin() + index, oldChild->keys[minDegree - 1]);
        parent->values.insert(parent->values.begin() + index, oldChild->values[minDegree - 1]);

        // Remove the duplicated entry from the split child
        oldChild->keys.erase(oldChild->keys.begin() + minDegree - 1);
        oldChild->values.erase(oldChild->values.begin() + minDegree - 1);

        parent->children.insert(parent->children.begin() + index + 1, newChild);
    }




    void inOrderTraversal(BTreeNode* node,string& tree) {
        if (node != nullptr) {
            int n = node->keys.size();
            int i;
            for (i = 0; i < n; i++) {
                if (!node->leaf) {
                    inOrderTraversal(node->children[i],tree);
                }
                tree =tree+ string(node->keys[i]) + " : " + node->values[i] + " ";
            }

            if (!node->leaf) {
                inOrderTraversal(node->children[i],tree);
            }
        }
    }

    void print(string &tree) {
        inOrderTraversal(root,tree);
    }

    BTreeNode* getRoot() const {
        return root;
    }
    void traverseAndMergeHelper(BTreeNode* currentNode, BTree* treeToMerge,BigInt& id,BigInt& currid,vector<string>& added,vector<string>& temp,vector<string>& newpath,BigInt& head,BigInt& last) {
        if (currentNode != nullptr) {
            for (int i = 0; i < currentNode->keys.size(); ++i) {
                if (currentNode->keys[i]<=id||(currentNode->keys[i]>last&&id==head))
                {
                    string replacement = "Machine_" + string(id);
                    string patternToReplace = "Machine_" + string(currid);
                    string updatedPath = currentNode->values[i];
                    size_t pos = updatedPath.find(patternToReplace);
                    if (pos != std::string::npos) {
                        updatedPath.replace(pos, patternToReplace.length(), replacement);
                    }
                    temp.push_back(currentNode->values[i]);
                    newpath.push_back(updatedPath);
                    for (int j = 0; j < added.size(); ++j) {
                        if (added[j] == currentNode->values[i]) {
                            added[j] = updatedPath;
                        }
                    }
                    treeToMerge->insert(currentNode->keys[i], updatedPath);
                    deleteKey(currentNode->keys[i], currentNode->values[i]);
                }
            }

            for (int i = 0; i < currentNode->children.size(); ++i) {
                traverseAndMergeHelper(currentNode->children[i],treeToMerge, id,currid, added, temp, newpath,head,last);
            }

            //delete currentNode;
        }
    }
    void CompareAndMerge(BTree* treeToMerge,BigInt& id,BigInt& currid,vector<string>& added,BigInt& head,BigInt& last) {
        vector<string> temp;
        vector<string> newpath;
        if (root != nullptr) {
            traverseAndMergeHelper(root, treeToMerge,id,currid,added,temp,newpath,head,last);
            for (int i = 0; i < temp.size(); i++)
            {
                fs::copy_file(temp[i], newpath[i], fs::copy_options::overwrite_existing);
                fs::remove(temp[i]);
            }
        }
    }
   void mergeTrees(BTree* treeToMergeInto, BTreeNode* currentNode,BigInt& id,BigInt& currid,vector<string>& added,vector<string>& temp,vector<string>& newpath) {
        if (currentNode != nullptr) {
            for (int i = 0; i < currentNode->keys.size(); ++i) {
                string replacement = "Machine_"+string(id);
                string patternToReplace = "Machine_"+string(currid);
                string updatedPath = currentNode->values[i];
                size_t pos = updatedPath.find(patternToReplace);
                if (pos != std::string::npos) {
                    updatedPath.replace(pos, patternToReplace.length(), replacement);
                }
                temp.push_back(currentNode->values[i]);
                newpath.push_back(updatedPath);
                for (int j = 0; j < added.size(); ++j) {
                    if (added[j] == currentNode->values[i]) {
                        added[j] = updatedPath;
                    }
                }
                treeToMergeInto->insert(currentNode->keys[i], updatedPath);
            }

            for (int i = 0; i < currentNode->children.size(); ++i) {
                mergeTrees(treeToMergeInto, currentNode->children[i],id,currid,added,temp,newpath);
            }

            delete currentNode;
        }
    }

    void emptyAndMerge(BTree* treeToMerge,BigInt& id,BigInt& currid,vector<string>& added) { 
        vector<string> temp;
        vector<string> newpath;
        mergeTrees(treeToMerge, root,id,currid,added,temp,newpath);
        for (int i = 0; i < temp.size(); i++)
        {
            fs::copy_file(temp[i], newpath[i], fs::copy_options::overwrite_existing);

        }
        root = nullptr; 
    }

};

//int main() {
//    int degree;
//    std::cout << "Enter the degree of the B-tree: ";
//    std::cin >> degree;
//
//    BTree btree(degree);
//
//    // Insert keys into the B-tree
//    btree.insert(BigInt("3"), "Value3");
//    btree.insert(BigInt("8"), "Value8");
//    btree.insert(BigInt("12"), "Value12");
//    btree.insert(BigInt("4"), "Value4");
//    btree.insert(BigInt("1"), "Value1");
//    btree.insert(BigInt("6"), "Value6");
//    btree.insert(BigInt("10"), "Value10");
//    btree.insert(BigInt("13"), "Value13");
//    btree.insert(BigInt("2"), "Value2");
//    btree.insert(BigInt("11"), "Value11");
//    btree.insert(BigInt("15"), "Value15");
//    btree.insert(BigInt("21"), "Value21");
//    btree.insert(BigInt("17"), "Value17");
//    btree.insert(BigInt("92"), "Value92");
//
//    // Print the B-tree
//    std::cout << "B-tree: ";
//    string a;
//    btree.print(a);
//    cout << a << endl;
//    btree.deleteKey(BigInt("6"),"6");
//    btree.deleteKey(BigInt("13"),"6");
//
//    btree.print();
//
//    // Search for keys in the B-tree
//    std::cout << "Search for key 6: " << (btree.search(btree.getRoot(), BigInt("6")) ? "Found" : "Not found") << std::endl;
//    std::cout << "Search for key 9: " << (btree.search(btree.getRoot(), BigInt("9")) ? "Found" : "Not found") << std::endl;
//
//    return 0;
//}
//
//
