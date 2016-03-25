#include<iostream>
#include<vector>

template < typename K, typename V, typename Compare = std::less < K > >
struct Node {
	Node * parent;
	Node * child;
	Node * next;
	int degree;
	K key;
	V value;

	Node(K _key, V _value) {
		parent = nullptr;
		child = nullptr;
		next = nullptr;
		degree = 0;
		key = _key;
		value = _value;
	}
};

template < typename K, typename V, typename Compare = std::less < K > >
void BinomialHeap_Link(Node <K, V> * first, Node <K, V> * second) { //соединяет 2 дерева одного размера
	first->parent = second;
	first->next = second->child;
	second->child = first; //вершина second становится родителем first и корнем
	++(second->degree);
}

template < typename K, typename V, typename Compare = std::less<K>>
class BinomialHeap {
	template < typename K, typename V, typename Compare = std::less < K > >
	friend BinomialHeap <K, V, Compare> BinomialHeap_union(BinomialHeap <K, V, Compare> * first, BinomialHeap < K, V, Compare> * second);
private:
	Node <K, V> * head;
	Compare comp;

public:
	BinomialHeap() : head(nullptr) {}

	Node <K, V> * BinomialHeap_min() {
		Node <K, V> * y = nullptr; // вершина,где достигается минимальное значение
		Node <K, V> * x = head;
		K min = 0;
		bool isMinINF = true;

		while (x != nullptr) {
			if (isMinINF || (comp(x->key, min))) {
				min = x->key;
				y = x;
				isMinINF = false;
			}
			x = x->next;
		}
		return y;
	}

	void BinomialHeap_insert(Node<K, V>* x) {
		BinomialHeap<K, V, Compare> H;
		x->parent = nullptr;
		x->next = nullptr;
		x->child = nullptr;
		x->degree = 0;
		H.head = x;
		*this = BinomialHeap_union<K, V>(this, & H);
	}

	Node <K, V> * BinomialHeap_extractMin() {//удаление вершины с минимальным ключом
		Node <K, V> * minNode = BinomialHeap_min();
		if (minNode == head) {
			head = head->next;
		}
		else {
			Node <K, V> * temp = head;
			while (temp->next != minNode) { //ищем указатель на вершину до минимума
				temp = temp->next;
			}
			temp->next = minNode->next;
		}

		Node <K, V> * children = minNode->child;
		if (children == nullptr) { //если вершина без детей,то сразу удаляем
			return minNode;
		}
		Node <K, V> * sibling = children->next;
		children->next = nullptr;
		children->parent = nullptr;
		while (sibling != nullptr) { // в корневом списке
			sibling = sibling->next;
			Node <K, V> * temp = children;
			children = children->next;
			children->next = temp;
			children->parent = nullptr;
		}
		BinomialHeap <K, V, Compare> * temp = new BinomialHeap <K, V, Compare >();
		temp->head = children;
		*this = BinomialHeap_union(this, temp);
		delete temp;
		return minNode;
	}
	void BinomialHeap_decreaseKey(Node<K, V>* node, K new_key) {
		if (comp(node->key, new_key)) {
			std::cerr << "New value of key is bigger then current";
			return;
		}
		node->key = new_key;
		BinomealHeap_decrease(node, new_key, false);
	}
	void BinomialHeap_deleteNode(Node <K, V> * node) {
		BinomialHeap_decrease(node, node->key, true);
		Node <K, V> * tempNode = BinomialHeap_extractMin();
		delete tempNode;
	}

private:
	void BinomealHeap_decrease(Node <K, V> * node, K new_node, bool isKeyInfin) {
		Node<K, V>* y = node;
		Node<K, V>* z = node->parent;
		while ((z != nullptr) && (isKeyInfin || comp(y->key, z->key))) {
			K tempKey = y->key;
			V tempValue = y->value;
			y->key = z->key;
			y->value = z->value;
			z->key = tempKey;
			z->value = tempValue;
			y = z;
			z = z->parent;
		}
	}
};

template < typename K, typename V, typename Compare = std::less < K > >
Node<K, V>* BinomealHeap_merge(Node <K, V>* first, Node<K, V>* second) {
	Node<K, V>* result;
	Node<K, V>* current;
	if (first != nullptr && (second == nullptr || first->degree < second->degree)) {
		result = first;
		first = first->next;
	}
	else {
		result = second;
		second = second->next;
	}
	current = result;
	while ((first != nullptr) && (second != nullptr)) {
		if (first->degree < second->degree) {
			current->next = first;
			first = first->next;
		}
		else {
			current->next = second;
			second = second->next;
		}
		current = current->next;
	}
	if (first != nullptr) {
		current->next = first;
	}

	if (second != nullptr) {
		current->next = second;
	}
	return result;
}
template < typename K, typename V, typename Compare = std::less < K > >
BinomialHeap <K, V, Compare> BinomialHeap_union(BinomialHeap <K, V, Compare> * first, BinomialHeap <K, V, Compare> * second) {
	BinomialHeap <K, V, Compare> * temp = new BinomialHeap <K, V, Compare>();
	temp->head = BinomealHeap_merge(first->head, second->head);
	if (temp->head == nullptr) {
		return *temp;
	}
	Node <K, V> * prev = nullptr;
	Node <K, V> * x = temp->head;
	Node <K, V> * subling = x->next;
	while (subling != nullptr) {
		if ((x->degree != subling->degree)
			|| ((subling->next != nullptr) && (subling->next->degree == x->degree))) {
			prev = x;
			x = subling;
		}
		else {
			if ((x->key < subling->key)) {
				x->next = subling->next;
				BinomialHeap_Link(subling, x);
			}
			else {	
				if (prev == nullptr) {
					temp->head = subling;
				}
				else {
					prev->next = subling;
				}
				BinomialHeap_Link(x, subling);
				x = subling;
			}
		}
		subling = x->next;
	}
	return *temp;
}


int main() {
	BinomialHeap <int, int>	heap1;
	BinomialHeap <int, int> heap2;
	Node <int, int> * Node1 = new Node <int, int>(400, 2);
	Node <int, int> * Node2 = new Node <int, int>(500, 40);
	Node <int, int> * Node3 = new Node <int, int>(300, 300);
	Node <int, int> * Node4 = new Node <int, int>(450, 55);
	Node <int, int> * Node5 = new Node <int, int>(100, 80);
	heap1.BinomialHeap_insert(Node1);
	heap1.BinomialHeap_insert(Node2);
	heap1.BinomialHeap_insert(Node3);
	heap2.BinomialHeap_insert(Node4);
	heap2.BinomialHeap_insert(Node5);

	Node<int,int> * min = heap1.BinomialHeap_min();
	std::cout << min->value << std::endl;
	heap1.BinomialHeap_extractMin();
	min = heap1.BinomialHeap_min();
	std::cout << min->value << std::endl;

	heap1.BinomialHeap_decreaseKey(Node2, 3);

	BinomialHeap <int, int> result = BinomialHeap_union(& heap1, &heap2);

	std::cout<<result.BinomialHeap_min()->value;
	

	return 0;
}