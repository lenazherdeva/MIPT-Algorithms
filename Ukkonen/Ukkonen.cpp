#include<string>
#include<map>
#include<locale>
#include<algorithm>

#include<iostream>

struct vertex;
struct edge {
	size_t beg;
	size_t lim;
	vertex * end;
	edge() :beg(0), lim(0), end(0){}
};
struct vertex {
	std::map<char, edge> child;
	size_t depth_dontuse;
	vertex * suffix_link;
	size_t id_dontUse;

};


void traverse(size_t d, edge * e) {
	size_t i;
	size_t lim;

	if (e->end) {
		if (e->end->depth_dontuse != d - e->lim - e->beg) {
			_asm int 3;
		}
		for (
			auto ch = e->end->child.begin(); // итератор по детям
			ch != e->end->child.end();
		ch++) {
			traverse(d + e->lim - e->beg,
				&(ch->second));
		}
	}
	else {
		//концевой вершины нет
		printf("%d\n", e->beg - d);
	}
}

void find_substring(
	std::string & s//текст,где ищем
	, edge *e, //суффиксное дерево для него
	std::string pattern) { //что ищем(целиком ищем)
	edge * cur_edge;
	size_t cur_edge_pos;
	size_t i = 0;
	cur_edge = e;
	cur_edge_pos = 0;
	while (i < pattern.length()) {
		if (cur_edge_pos < cur_edge->lim) {
			//стоим на ребре
			//пытаемся сравнить очередные символы
			if (s[cur_edge_pos] == pattern[i]) {
				i++;
				cur_edge_pos++;
			}
			else {
				return;
			}
		}
		else {
			//стоим в конце текущего ребра, т е в его концевой вершине
			auto out = cur_edge->end->child.find(pattern[i]);// ищем полходящее исходящее ребро
			if (out == cur_edge->end->child.end()) {
				return;
			}
			cur_edge = &(out->second);
			cur_edge_pos = cur_edge->beg;

		}

	}
	//подстрока найдена. перебрать все концевые вершины

	traverse(i - cur_edge_pos + cur_edge->beg, cur_edge);
}




void print_edge(size_t shift, std::string &s, edge * e) {
	size_t i;
	size_t lim;
	//выводим отступ
	for (int i = 0; i < shift; i++) printf(" ");
	//выводим текст ребпра
	if (e->end)
		lim = e->lim;
	else
		lim = s.length();

	for (i = e->beg; i < lim; i++) printf("%c", s[i]); //символы самого ребра
	//есть концевая вершина выводим дочерние ребра
	if (e->end) {
		//printf("\n");
		printf("(depth:%d; id: %d; link: %d)\n", e->end->depth_dontuse, e->end->id_dontUse,
			e->end->suffix_link ? (e->end->suffix_link->id_dontUse) : -1);
		for (
			auto ch = e->end->child.begin(); // итератор по детям
			ch != e->end->child.end();
		ch++) {
			print_edge(
				shift + lim - e->beg, s, &(ch->second));
		}
	}
	else{
		//концевой вершины нет
		printf("$\n");
	}
}

int main() {
	std::string s;
	//s = "abababaab";
	s = "ojojaojojb";
	//setlocale(LC_ALL, "Russian");

	printf("%s\n", s.c_str());
	vertex root;
	edge edge_to_root; //= { 0, 0, &root };
	size_t cur_suf;
	size_t cur_suf_pos;
	edge * cur_edge;
	size_t cur_edge_pos;
	size_t depth; //текущая глубина
	vertex * no_link_vertex; //последняя созданая вершина, у которой еще нет стрелки
	size_t no_link_depth; //ее глубина
	size_t last_linked_depth;
	vertex * last_linked_vertex;
	edge fictive_edge;



	root.depth_dontuse = 0;
	root.suffix_link = 0;
	cur_suf = cur_suf_pos = 0;
	size_t vertex_id = 0;
	cur_edge = &edge_to_root;
	cur_edge_pos = 0;
	edge_to_root.beg = 0;
	edge_to_root.lim = 0;
	edge_to_root.end = &root;
	depth = 0;
	root.id_dontUse = 0;
	no_link_vertex = 0;
	no_link_depth = -1;
	last_linked_depth = -1;
	last_linked_vertex = 0;

	for (size_t len = 1; len <= s.length(); len++) {
		while (cur_suf_pos < len) {
			char ch = s[cur_suf_pos];
			if (cur_edge_pos < cur_edge->lim) { //не в конце и могу шагнуть
				//можем ли сдлеать по нему шаг
				if (no_link_vertex) {
					size_t jump;
					jump = std::min(no_link_depth - 1 - depth,
						cur_edge->lim - cur_edge_pos); //прыгаем если тоже самое //предварительно проверяем, может спустимся
					if (jump) {
						cur_suf_pos += jump;
						cur_edge_pos += jump;
						depth += jump;
						continue;
					}

				}

				if (ch == s[cur_edge_pos]) {
					//очередные символы совпадают-делаем шаг
					cur_suf_pos++;
					cur_edge_pos++;
					depth++;
					continue;
				}
				//стоим на ребре но текущие символы не совпадают
				//создаем новую вершину
				vertex *nv = new vertex;
				//инициализация единственного исходящего ребра из новой вершины
				nv->child[s[cur_edge_pos]].beg = cur_edge_pos;
				nv->child[s[cur_edge_pos]].lim = cur_edge->lim;
				nv->child[s[cur_edge_pos]].end = cur_edge->end;
				nv->suffix_link = 0;
				nv->depth_dontuse = depth; //просто для дебага
				nv->id_dontUse = ++vertex_id;

				//укорачиваем старое ребро
				cur_edge->lim = cur_edge_pos;
				cur_edge->end = nv;
				if (no_link_vertex) {//не есть ли это та вершину, в которую надо поставить стрелку
					if (no_link_depth != depth + 1) {
						_asm int 3;
					}
					no_link_vertex->suffix_link = nv;

				}


				no_link_vertex = nv;
				no_link_depth = depth;
			}
			//попав сюда, мы стоим в концевой вершине ребра
			if (cur_edge->end->suffix_link) {
				last_linked_vertex = cur_edge->end;
				last_linked_depth = depth;
			}

			if (no_link_vertex && (no_link_depth == depth + 1)) {//не есть ли это та вершину, в которую надо поставить стрелку
				no_link_vertex->suffix_link = cur_edge->end;
				no_link_vertex = 0;
				no_link_depth = 0;
			}

			cur_edge = &(cur_edge->end->child[ch]); //исходящее ребро из конца текущего, если его нет-оно заводится//вышли из вершины по ch
			if (cur_edge->lim) {
				//встали на существующее ребро
				cur_edge_pos = cur_edge->beg;
				continue;
			}
			else {
				//создали новое
				cur_edge->beg = cur_suf_pos;
				cur_edge->lim = size_t(-1); //бесконечность
				cur_edge->end = 0;
				//текущий суффикс получил свой лист переходим к следующему суффиксу
				cur_suf++;
				cur_suf_pos = cur_suf;
				if (last_linked_vertex) {
					//Прыгаем по суффиксной стрелке к вершине след суффикса
					last_linked_vertex = last_linked_vertex->suffix_link;
					last_linked_depth--;

					cur_suf_pos += last_linked_depth;

					fictive_edge.beg = 0;//стоим в вершине ласт
					fictive_edge.lim = 0;
					fictive_edge.end = last_linked_vertex;

					cur_edge = &fictive_edge;
					cur_edge_pos = 0;

					depth = last_linked_depth;

					//если в вершине в которую мы прыгнули, нет суффиксн ссылки
					last_linked_vertex = 0;
					last_linked_depth = -1;

					continue;
				}

				//в корень дерева
				cur_edge = &edge_to_root;
				cur_edge_pos = 0;
				depth = 0;
				continue;
			}

		}
	}

	//print_edge(0, s, &edge_to_root);

	while (true) {
		std::string p;
		std::cin >> p;
		if (p==std::string("exit")) break;
		find_substring(s, &edge_to_root, p);
	}
	return 0;
}