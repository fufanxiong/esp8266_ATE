#ifndef COMMHEAD_H
#define COMMHEAD_H


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename T>
class CdNode
{
private:
	CdNode& operator = (const CdNode &x);
	CdNode(CdNode &x);
public:
	CdNode *prev;
	CdNode *next;
	T *cur;
	CdNode(){prev = next = this;cur = NULL;};
	virtual ~CdNode() {};
	void add_tail(CdNode* &head);
	void del(CdNode* &head);
};

template <typename T>
void CdNode<T>::add_tail(CdNode* &head)
{
    if(head) {
        prev = head->prev;
        next = head;
        prev->next = this;
        head->prev = this;
    }else head = this;
}

template <typename T>
void CdNode<T>::del(CdNode* &head)
{
	if(head == this) {
		if(head->next == this) head = NULL;
		else head = head->next;
	}

	next->prev = prev;
	prev->next = next;
	next = this;
	prev = this;
}

struct bt_address_s 
{
	bool is_valid;
	int nap;
	int uap;
	int lap;
	CString mac_addr;
};




#endif //COMMHEAD_H










