// 1_6_1
// Построение и обработка бинарного дерева поиска

#include <iostream> 
#include <string>
#include <vector> 
#include <stdexcept>
#include <random>
#include <regex>

using namespace std::string_literals;

#define HINT(hint) std::cout << hint << std::endl;
#define ERROR_HINT(hint) std::cout << "Ошибка. "s << hint << std::endl;

static bool IsRangeNum(const std::string& string, const char limit_begin, const char limit_end) {
	if (string.empty()) {
		return false;
	}
	else {
		for (const char& c : string) {
			if (c < limit_begin || c > limit_end) {
				return false;
			}
		}
		return true;
	}
}

static bool IsIntNum(const std::string& str) {
	static std::regex num_reg("\\s*([+-]?[0-9]+)\\s*");
	return regex_match(str, num_reg);
}

template <typename T>
T GetRandomNum(const T min, const T max) {
	static std::random_device rd;
	return std::uniform_int_distribution{ min, max }(rd);
}

template <typename Type>
struct Node {
	Node() = default;
	Node(const Type& val, Node<Type>* new_lhs = nullptr, Node<Type>* new_rhs = nullptr)
		: value(val)
		, number_of_copies(1)
		, lhs(new_lhs)
		, rhs(new_rhs) {
	}
	Type value;
	int number_of_copies;
	Node<Type>* lhs; // left-hand side
	Node<Type>* rhs; // right-hand side
};

template <typename Type>
struct BinaryTree {
private:
	Node<Type>* root_;
	size_t size_;
public:
	BinaryTree()
		: root_()
		, size_(0)
	{}

	Node<Type>* SetRoot(Node<Type>* root) {
		root_ = root;
		return root_;
	}

	Node<Type>* GetRoot() {
		return root_;
	}

	// Вывод всех вершин в одну строку в порядке следования ключей
	void Print(Node<Type>* root) {
		if (root == nullptr) {
			return;
		}
		Print(root->lhs);
		std::cout << root->value << "("s << root->number_of_copies << ")"s << "\t";
		Print(root->rhs);
	}

	// Прямой обход
	//void PrintForward(Node<Type>* root, int tmp = 0) {
	//	if (root != nullptr) {

	//		std::string str;
	//		for (int i = 0; i < tmp; i++) {
	//			str += "\t"s;
	//		}
	//		std::cout << str << root->value; 
	//		std::cout << std::endl; 

	//		PrintForward(root->lhs, tmp + 1);
	//		PrintForward(root->rhs, tmp + 1);
	//	}
	//}

	// Рекурсивный симметричный обход
	//void PrintSymmetrical(Node<Type>* root, int tmp = 0) {
	//	if (root != nullptr) {
	//		PrintSymmetrical(root->lhs, tmp + 1);

	//		std::string str;
	//		for (int i = 0; i < tmp; i++) {
	//			str += "\t"s;
	//		}
	//		std::cout << str << root->value; 
	//		std::cout << std::endl; 

	//		PrintSymmetrical(root->rhs, tmp + 1);
	//	}
	//} 

	// Обратно-симметричный обход
	void PrintInverselySymmetrical(Node<Type>* root, int depth = 0, bool isRoot = true) {
		if (root != nullptr) {
			// Определяем количество отступов для узлов на первом уровне
			int indent = isRoot ? 4 : 1;

			// Вывод правого поддерева
			PrintInverselySymmetrical(root->rhs, depth + 1, false);

			// Выводим отступы в зависимости от глубины узла
			for (int i = 0; i < depth; i++) {
				std::cout << "\t";
			}

			// Вывод значения узла
			std::cout << root->value;
			if (root->number_of_copies > 1) {
				std::cout << "(" << root->number_of_copies << ")";
			}
			std::cout << std::endl;

			// Если это корень, добавляем дополнительные пробелы между поддеревьями
			if (isRoot && root->lhs) {
				for (int i = 0; i < indent; i++) {
					std::cout << std::endl;
				}
			}

			// Вывод левого поддерева
			PrintInverselySymmetrical(root->lhs, depth + 1, false);
		}
	}

	enum class InputType {
		_EXIT_,
		_PEEK_,
		_PUSH_,
		_DELETE_,
		_PRINT_,
		_WAIT_CHOISE_
	} input_type_ = InputType::_WAIT_CHOISE_;

	enum class PushType {
		_PUSH_ROOT_,
		_PUSH_LHS_,
		_PUSH_RHS_,
	};

	// Проверить, имеет ли дерево корень.
	bool IsEmpty() const {
		return root_ == nullptr;
	}

	// Проверить, является ли предок parent терминальным узлом.
	bool IsTerminalNode(Node<Type>* parent) const {
		return (parent->lhs == nullptr) && (parent->rhs == nullptr);
	}

	// Проверить, имеет ли предок parent свободного потомка lhs или rhs.
	bool IsFreeNode(Node<Type>* parent) const {
		return (parent->lhs == nullptr) || (parent->rhs == nullptr);
	}

	// Функция построения бинарного дерева поиска
	Node<Type>* Push(Node<Type>* root, Type new_value) {
		if (root == nullptr) {
			return new Node<Type>(new_value);
		}
		else if (new_value < root->value) {
			root->lhs = Push(root->lhs, new_value);
		}
		else if (new_value > root->value) {
			root->rhs = Push(root->rhs, new_value);
		}
		else if (new_value == root->value) {
			++root->number_of_copies;
		}
		return root;
	}

	Node<Type>* PushNonRecursive(Node<Type>* root, Type new_value) {
		if (root == nullptr) {
			return new Node<Type>(new_value);
		}
		Node<Type>* current = root;
		while (true) {
			if (new_value < current->value) {
				if (current->lhs == nullptr) {
					current->lhs = new Node<Type>(new_value);
					break;
				}
				else {
					current = current->lhs;
				}
			}
			else if (new_value > current->value) {
				if (current->rhs == nullptr) {
					current->rhs = new Node<Type>(new_value);
					break;
				}
				else {
					current = current->rhs;
				}
			}
			else if (new_value == current->value) {
				++current->number_of_copies;
				break;
			}
		}

		return root;
	}
	Node<Type>* Pop(Node<Type>* node, Type val) {
		if (node == nullptr) {
			return nullptr; // Узел не найден, ничего не делаем
		}

		if (val == node->value) {
			if (node->lhs == nullptr && node->rhs == nullptr) {
				// Узел является листом
				delete node;
				return nullptr;
			}
			if (node->lhs == nullptr and node->rhs != nullptr) {
				// Узел имеет только правого потомка
				Node<Type>* tmp = node->rhs;
				delete node;
				return tmp;
			}
			if (node->rhs == nullptr and node->lhs != nullptr) {
				// Узел имеет только левого потомка
				Node<Type>* tmp = node->lhs;
				delete node;
				return tmp;
			}

			// Узел имеет двух потомков
			// Найдем наименьший узел в правом поддереве
			Node<Type>* ptr = node->rhs;
			Node<Type>* pmin = ptr;

			// Поиск наименьшего узла в правом поддереве
			while (pmin->lhs != nullptr) {
				ptr = pmin;
				pmin = ptr->lhs;
			}

			// Заменяем значения
			node->value = pmin->value;

			// Удаляем наименьший узел в правом поддереве
			node->rhs = Pop(node->rhs, pmin->value);
		}
		else if (val < node->value) {
			node->lhs = Pop(node->lhs, val);
		}
		else {
			node->rhs = Pop(node->rhs, val);
		}

		return node;
	}

	Node<Type>* Peek(Node<Type>* parent, const Type value) noexcept {
		if (parent == nullptr) {
			return nullptr;  // Если текущий узел равен nullptr, значит элемент не найден
		}
		if (parent->value == value) {
			return parent;  // Элемент найден, возвращаем текущий узел
		}
		// Проверяем дочерние узлы
		Node<Type>* found_in_lhs = Peek(parent->lhs, value);
		if (found_in_lhs != nullptr) {
			return found_in_lhs;  // Элемент найден в левом дочернем узле
		}
		Node<Type>* found_in_rhs = Peek(parent->rhs, value);
		if (found_in_rhs != nullptr) {
			return found_in_rhs;  // Элемент найден в правом дочернем узле
		}
		return nullptr;  // Элемент не найден в текущем узле и его дочерних узлах
	}

	void Clear(Node<Type>* parent) {
		if (parent != nullptr) {
			Clear(parent->lhs);
			Clear(parent->rhs);
			if (parent == root_) {
				root_ = nullptr;
			}
			delete parent;
		}
	}

	void Print() {
		if (IsEmpty()) {
			ERROR_HINT("Невозможно произвести вывод: дерево не имеет узлов."s);
			return;
		}
		else {
			/*std::cout << "Прямой обход:"s << std::endl;
			PrintForward(GetRoot(), 0);

			std::cout << "Симметричный обход:"s << std::endl;
			PrintSymmetrical(GetRoot(), 0);*/

			std::cout << "Обратно-симметричный обход:"s << std::endl;
			PrintInverselySymmetrical(GetRoot(), 0);

			std::cout << "Вывод всех вершин по порядку следования ключей:"s << std::endl;
			Print(GetRoot());
		}
	}

	void Menu() {
		std::cout << "УКАЖИТЕ НОМЕР КОМАНДЫ"s << std::endl;
		do {
			std::cout << "1 >> Поиск заданного значения"s << std::endl;
			std::cout << "2 >> Добавление"s << std::endl;
			std::cout << "3 >> Удаление"s << std::endl;
			std::cout << "4 >> Вывод на экран"s << std::endl;
			std::cout << "0 >> Завершение работы"s << std::endl;
			std::string user_input;
			std::getline(std::cin, user_input);

			if (IsRangeNum(user_input, '0', '5')) {
				input_type_ = static_cast<InputType>(std::stoi(user_input));
				switch (input_type_) {
				case BinaryTree::InputType::_EXIT_: {
					HINT("Session Killed by User Request."s);
					break;
				}

				case BinaryTree::InputType::_PEEK_: {
					if (IsEmpty()) {
						ERROR_HINT("Невозможно выполнить поиск: источник пуст.");
					}
					else {
						do {
							std::cout << "Укажите значение: "s;
							std::string parent_value;
							std::getline(std::cin, parent_value);
							if (IsIntNum(parent_value)) {
								Node<Type>* tmp = Peek(root_, std::stoi(parent_value));
								if (tmp != nullptr) {
									HINT("Узел со значением ("s + parent_value + ") существует."s);
									if (IsTerminalNode(tmp)) {
										HINT("Является терминальным узлом."s);
									}
									else {
										HINT("Не является терминальным узлом."s);
									}
								}
								else {
									HINT("Узел со значением ("s + parent_value + ") не существует."s);
								}
								break;
							}
						} while (true);
					}
					break;
				}

				case BinaryTree::InputType::_PUSH_: {
					bool HasBeenAdded{ false };
					if (IsEmpty()) {
						do {
							HINT("1 >> Заполнить случайными значениями"s);
							HINT("2 >> Добавить вершины самостоятельно"s);
							std::string input_type;
							std::getline(std::cin, input_type);
							size_t push_type;
							if (IsIntNum(input_type)) {
								if (std::stoi(input_type) == 1) {
									HINT("1 >> Добавление рекурсивной функцией"s);
									HINT("2 >> Добавление нерекурсивной функцией"s);

									do {
										std::string tmp_push_type;
										std::getline(std::cin, tmp_push_type);
										if (IsRangeNum(tmp_push_type, '1', '2')) {
											if (std::stoi(tmp_push_type) == 1) {
												push_type = static_cast<int>(std::stoi(tmp_push_type));
											}
											else if (std::stoi(tmp_push_type) == 2) {
												push_type = static_cast<int>(std::stoi(tmp_push_type));
											}
											break;
										}
										else {
											ERROR_HINT("Ожидается натуральное число.");
										}
									} while (true);
									do {
										std::cout << "Укажите количество узлов: "s;
										std::string size;
										std::getline(std::cin, size);
										if (IsIntNum(size)) {
											size_ = std::stoi(size);
											for (size_t i = 0; i < size_; ++i) {
												if (push_type == 1) {
													SetRoot(Push(root_, GetRandomNum(0, 99)));
												}
												else if (push_type == 2) {
													SetRoot(PushNonRecursive(root_, GetRandomNum(0, 99)));
												}
											}
											HINT("Операция добавления выполнена."s);
											// Сообщаем, что добавление уже было произведено 
											// и не предлагаем указать вершины самостоятельно.
											HasBeenAdded = true;
											break;
										}
										else {
											ERROR_HINT("Ожидается натуральное число.");
										}
									} while (true);
								}
								else if (std::stoi(input_type) == 2) {
									break;
								}
								break;
							}
							else {
								ERROR_HINT("Ожидается натуральное число.");
							}
						} while (true);
					}
					if (!HasBeenAdded) {
						do {
							std::cout << "Укажите новое значение: "s;
							std::string value;
							std::getline(std::cin, value);
							if (IsIntNum(value)) {
								SetRoot(Push(root_, std::stoi(value)));
								HINT("Операция добавления выполнена."s);
								break;
							}
							else {
								ERROR_HINT("Ожидается целочисленное рациональное число.");
							}
						} while (true);
					}
					break;
				}

				case BinaryTree::InputType::_DELETE_: {
					do {
						std::cout << "Укажите значение: "s;
						std::string value;
						std::getline(std::cin, value);
						if (IsIntNum(value)) {
							Node<Type>* parent = Peek(root_, std::stoi(value));
							if (parent != nullptr) {
								Pop(root_, std::stoi(value));
								HINT("Операция удаления выполнена."s);
								break;
							}
							else {
								ERROR_HINT("Узела с заданным значением не существует."s);
							}
							break;
						}
						else {
							ERROR_HINT("Ожидается целочисленное рациональное число.");
						}
					} while (true);
					break;
				}

				case BinaryTree::InputType::_PRINT_: {
					Print();
					break;
				}

				default:
					break;
				}
			}
			else {
				ERROR_HINT("Некорректный ввод.");
			}
			std::cout << std::endl;
		} while (input_type_ != InputType::_EXIT_);
	}
};

int main() {
	setlocale(LC_ALL, "RUS");
	BinaryTree<int> tree;
	tree.Menu();
	return 0;
}