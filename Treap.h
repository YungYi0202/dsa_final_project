#pragma once
#include <stdlib.h>
#include<exception>
//#include <vector>
#include<list>
#include"Mail.h"

using namespace std;

typedef std::list<Mail>::iterator Elemt_Iter;//to be rewrited to template

class TreapNode{
public:
    Elemt_Iter val; //結點值
private:
    // int priority;   //優先順序
    TreapNode* lchild;
    TreapNode* rchild;
    TreapNode(const Elemt_Iter& val) : val(val), lchild(NULL), rchild(NULL){}
    friend class Treap;
};

typedef TreapNode* Tree;

bool priority_less(const Elemt_Iter& a, const Elemt_Iter& b){ return Mail::priority_less((*a), (*b)); }


class Treap{//max-heap
public:
    Treap():root(NULL), num(0){}
    Tree search(const Elemt_Iter&);
    Tree search(int key_value);
    bool insert(const Elemt_Iter&);
    Elemt_Iter remove(int key_value);
    const Elemt_Iter& getMax(); 
    int Num(){return num;}
    void Print();
	bool empty(){return num == 0;}
private:
    Tree root;
    int num;
    bool insert_val(Tree &,Tree &);
    void right_rotate(Tree&);
    void left_rotate(Tree &);
    Elemt_Iter remove_val(Tree &, int key_value);
    void PreOrder(Tree);
    void InOrder(Tree);
};

/*======================================================*/


//查詢函式
Tree Treap::search(const Elemt_Iter& val){
    Tree ptr = root;
    while(ptr != NULL){
        if(*val == *( ptr->val)) return ptr;
        else if(*val < *(ptr->val)) ptr = ptr->lchild;
        else ptr = ptr->rchild;
    }
    return NULL;
}
Tree Treap::search(int key_value){
	Tree ptr = root;
	while(ptr != NULL){
        if(key_value == ptr->val->getid()) return ptr;
        else if(key_value < ptr->val->getid()) ptr = ptr->lchild;
        else ptr = ptr->rchild;
    }
    return NULL;
}


//插入函式
bool Treap::insert(const Elemt_Iter& val){
    Tree node=new TreapNode(val);
    bool success = insert_val(root, node);
    if(!success) delete node;
    return success;
}
//內部插入函式
//引數:根結點，需插入的結點
bool Treap::insert_val(Tree &root,Tree &node){
    if (!root){
        root=node; //插入
        num ++; // successfully insert
        return true;
    }
    else if(*(root->val) > *(node->val)){//go left
        bool flag=insert_val(root->lchild, node);
		if(priority_less(root->val, node->val)){//檢查是否需要調整max heap
			right_rotate(root);
		}
        return flag;
    }
    else if(*(root->val) < *(node->val)){//go right
        bool flag=insert_val(root->rchild, node);
		if(priority_less(root->val, node->val)){//檢查是否需要調整max heap
			left_rotate(root);
		}
        return flag;
    }
    //已經含有該元素，釋放結點
    //delete node;    
    return false;

}
//右旋轉
void Treap::right_rotate(Tree &node){//DONE
    Tree temp=node->lchild;
    node->lchild=temp->rchild;
    temp->rchild=node;
    node=temp;
}
//左旋轉
void Treap::left_rotate(Tree &node){//DONE
    Tree temp=node->rchild;
    node->rchild=temp->lchild;
    temp->lchild=node;
    node=temp;
}

//刪除函式
Elemt_Iter Treap::remove(int key_value){//fault
    return remove_val(root, key_value); 
}

Elemt_Iter Treap::remove_val(Tree &root, int key_value){
    if (root==NULL)
        throw std::logic_error("wanted removed mail is not present");
    else if (root->val->getid() > key_value)
        return remove_val(root->lchild, key_value);
    else if(root->val->getid() < key_value)
        return remove_val(root->rchild, key_value);
    else{//找到執行刪除處理
		Elemt_Iter return_value = root->val;
        Tree *node=&root;
        while ((*node)->lchild && (*node)->rchild){//從該結點開始往下調整
			if(priority_less((*node)->rchild->val, (*node)->lchild->val)){//比較其左右孩子優先順序
                right_rotate(*node); //右旋轉
                node=&((*node)->rchild); //更新傳入引數，進入下一層
			}
            else{
                left_rotate(*node); //左旋轉
                node=&((*node)->lchild); //更新傳入引數，進入下一層
            }
        }
        //最後調整到（或者本來是）葉子結點，或者只有一個孩子的情況，可以直接刪除了
        if ((*node)->lchild==NULL)
            (*node)=(*node)->rchild;
        else if((*node)->rchild==NULL)
            (*node)=(*node)->lchild;

        num--;//successfully remove
        return return_value;
    }
}
const Elemt_Iter& Treap::getMax(){
    return root->val;
}
void Treap::Print(){
	std::cout << "(id, alphNum_cnt)\n";
    printf("PreOrder:\n");
    PreOrder(root);
    printf("\n");
    printf("--------------------\n");
    printf("InOrder\n");
    InOrder(root);
    printf("\n");

}
//前序
void Treap::PreOrder(Tree root)
{
    if (root==NULL)
        return;
	std::cout << "(" << root->val->getid() << ", " << root->val->get_alphNum_cnt() << ") ";
    PreOrder(root->lchild);
    PreOrder(root->rchild);
}
//中序
void Treap::InOrder(Tree root)
{
    if (root==NULL)
        return;
    InOrder(root->lchild);
	std::cout << "(" << root->val->getid() << ", " << root->val->get_alphNum_cnt() << ") ";
    InOrder(root->rchild);
}