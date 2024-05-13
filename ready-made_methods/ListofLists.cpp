#include <iostream> 
#include <string>
#include <vector> 
#include <stdexcept>

/* Структура данных список списков. На основе односвязного списка, основной список и каждый из подсписков имеют фиктивные узлы. Не закольцован ни основной, ни подсписки. */

using namespace std::string_literals;

#define HINT(hint) std::cout << hint << std::endl;
#define ERROR_HINT(hint) std::cout << "Ошибка. "s << hint << std::endl;

static bool isOnlyNumbers(const std::string& string, const char limit_begin, const char limit_end) {
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

template <typename Type>
struct SubNode {
	SubNode() = default;
	SubNode(const Type& val, SubNode<Type>* next)
		: value(val)
		, size(0)
		, next_node(next)
	{}
	Type value;
	size_t size;
	SubNode<Type>* next_node;
};

template <typename Type>
struct Node {
	Node() = default;
	Node(const Type& val, Node<Type>* next)
		: value(val)
		, size(0)
		, next_node(next)
	{}
	Type value;
	size_t size;
	Node<Type>* next_node;
	SubNode<Type> head = SubNode<Type>(-1, nullptr);
};

template <typename Type>
struct SinglyLinkedList {
private:
	Node<Type> head_;

public:
	SinglyLinkedList() {
		head_ = Node<Type>(-1, nullptr);
	}

	~SinglyLinkedList() {
		while (head_.next_node != nullptr) {
			Node<Type>* tmp = head_.next_node; // указатель на первый элемент списка 
			Clear(&tmp->head);
			head_.next_node = head_.next_node->next_node; // делаем так, чтобы 1й узел ссылался теперь на 2й узел 
			delete tmp;
			--head_.size;
		}
	}

	enum class MenuStatus {
		EXIT_,
		PEEK_,
		APPEND_,
		DELETE_,
		PRINT_,
		WAIT_CHOISE
	} status_ = MenuStatus::WAIT_CHOISE;

	enum class OperationStatus {
		PUSH_BACK = 1,
		PUSH_BEFORE = 2,
		PUSH_AFTER = 3,
		DELETE_NOT_ALL_VALUES,
		DELETE_SUBNODE
	};

	template <typename T>
	bool IsEmpty(T* head) const {
		return head->next_node == nullptr;
	}

	// Возвращает указатель на последний узел в списке
	template <typename T>
	T* GetBack(T* head) {
		T* tmp = head;
		while (tmp->next_node != nullptr) {
			tmp = tmp->next_node;
		}
		return tmp;
	}

	template <typename Pos>
	void DoPush(const std::vector<int>& new_values, Pos* pos, OperationStatus status) {
		switch (status) {
		case SinglyLinkedList::OperationStatus::PUSH_BACK: {
			for (const auto& new_value : new_values) {
				Pos* new_element = new Pos(new_value, nullptr);
				pos->next_node = new_element;
				pos = new_element;
			}
			break;
		}
		case SinglyLinkedList::OperationStatus::PUSH_BEFORE: {
			for (const auto& new_value : new_values) {
				Pos* new_element = new Pos(new_value, pos->next_node);
				pos->next_node = new_element;
				pos = new_element;
			}
			break;
		}
		case SinglyLinkedList::OperationStatus::PUSH_AFTER: {
			for (const auto& new_value : new_values) {
				Pos* new_element = new Pos(new_value, pos->next_node);
				pos->next_node = new_element;
				pos = new_element;
			}
			break;
		}
		default:
			break;
		}
	}

	template <typename T>
	void Push(const std::vector<Type> nums, T* pos, T* head) {
		try {
			switch (SelectPushType()) {
			case OperationStatus::PUSH_BACK: {
				pos = GetBack(head);
				DoPush(nums, pos, OperationStatus::PUSH_BACK);
				head->size = nums.size();
				break;
			}
			case OperationStatus::PUSH_BEFORE: {
				if (IsEmpty(head)) {
					throw std::invalid_argument("Невозможно выполнить вставку 'перед': источник пуст.");
				}
				else {
					std::cout << "Укажите значение, перед которым нужно выполнить вставку: "s;
					do {
						std::string found_node;
						std::getline(std::cin, found_node);
						if (isOnlyNumbers(found_node, '0', '9')) {
							pos = PeekBefore(head, std::stoi(found_node));
							DoPush(nums, pos, OperationStatus::PUSH_BEFORE);
							head->size = nums.size();
							break;
						}
					} while (true);
					break;
				}
			}
			case OperationStatus::PUSH_AFTER: {
				if (IsEmpty(head)) {
					pos = head;
				}
				else {
					do {
						std::cout << "Укажите значение, после которого нужно выполнить вставку: "s;
						std::string found_node;
						std::getline(std::cin, found_node);
						if (isOnlyNumbers(found_node, '0', '9')) {
							pos = Peek(head, std::stoi(found_node));
							head->size = nums.size();
							break;
						}
					} while (true);
				}
				DoPush(nums, pos, OperationStatus::PUSH_AFTER);
				break;
			}
			default:
				break;
			}
			std::cout << "Добавление выполнено успешно."s << std::endl;

		}
		catch (const std::exception& error) {
			ERROR_HINT(error.what());
		}
	}

	void Delete(const int found_element, const int found_index, const OperationStatus& status) {
		if (IsEmpty(&head_)) {
			throw std::out_of_range("Невозможно произвести удаление: источник пуст."s);
		}
		else {
			switch (status) {
			case OperationStatus::DELETE_NOT_ALL_VALUES: {

				// Возвращает указатель на узел основного списка, индекс которого равен index
				Node<Type>* found_node = Peek(&head_, found_index);

				bool isHasBeenDelete{ false };
				try {
					// Возвращает указатель на узел, значение которого равно found_element
					SubNode<Type>* found_subnode = nullptr; // Peek(&found_node->head, found_element);
					while ((found_subnode = Peek(&found_node->head, found_element)) != nullptr) {
						SubNode<Type>* tmp = &found_node->head;
						while (tmp->next_node != found_subnode) {
							tmp = tmp->next_node;
						}
						tmp->next_node = found_subnode->next_node; // Пропускаем найденный узел
						--found_node->size;
						delete found_subnode;
						isHasBeenDelete = true;
					}
				}
				catch (const std::exception& info) {
					//continue;
				}
				if (!isHasBeenDelete) {
					throw std::exception("Узлы с заданным значением не найдены.");
				}

				break;
			}

			case OperationStatus::DELETE_SUBNODE: {
				Node<Type>* found_node = Peek(&head_, found_index);

				// Очистка подсписка
				SubNode<Type>* current = &found_node->head;
				Clear(current);
				found_node->size = 0;

				// Удаление самого узла, содержащий подсписок
				Node<Type>* current_head = &head_;
				while (current_head->next_node != found_node) {
					current_head = current_head->next_node;
				}
				current_head->next_node = found_node->next_node; // Пропускаем найденный узел
				delete found_node;
				--head_.size;
				break;
			}
			default:
				break;
			}
		}
	}

	void Clear(SubNode<Type>* current_head_sub) {
		while (current_head_sub->next_node != nullptr) {
			SubNode<Type>* temp = current_head_sub->next_node; // указатель на первый элемент списка 
			current_head_sub->next_node = current_head_sub->next_node->next_node; // делаем так, чтобы 1й узел ссылался теперь на 2й узел
			--current_head_sub->size;
			delete temp;
		}
	}

	// Возвращает указатель на узел, у которого value равно found_value. Возвращает nullptr, если узел не найден
	template <typename T>
	T* PeekNoexcept(T* current_head, const int found_value) const noexcept {
		if (IsEmpty(current_head)) {
			return nullptr;
		}
		else {
			T* current = current_head; // начинает с фиктивного узла 
			while (current != nullptr) {
				if (current->value == found_value) {
					return current;
				}
				current = current->next_node;
			}
			throw nullptr;
		}
	}

	// Возвращает указатель на узел, у которого value равно found_value. Бросает исключение, если узел не найден 
	template <typename T>
	T* Peek(T* current_head, const int found_value) const {
		if (IsEmpty(current_head)) {
			throw std::invalid_argument("Невозможно выполнить поиск: источник пуст."s);
		}
		else {
			T* current = current_head; // начинает с фиктивного узла 
			while (current != nullptr) {
				if (current->value == found_value) {
					return current;
				}
				current = current->next_node;
			}
			throw std::invalid_argument("Узел не найден."s);
		}
	}

	// Возвращает указатель на узел, у которого value равно found_value
	template <typename T>
	T* PeekBefore(T* current_head, const int found_value) const {
		if (IsEmpty(current_head)) {
			throw std::invalid_argument("Невозможно выполнить поиск: источник пуст."s);
		}
		else {
			T* current = current_head;
			while (current->next_node != nullptr) {
				if (current->next_node->value == found_value) {
					return current;
				}
				current = current->next_node;
			}
			throw std::invalid_argument("Узел не найден."s);
		}
	}

	// Возвращает вектор индексов подсписков, в которых содержится found_node
	std::vector<int> IsExists(Node<Type>* current_head, const int& found_node) const {
		if (IsEmpty(current_head)) {
			throw std::invalid_argument("Невозможно выполнить поиск: источник пуст."s);
		}
		else {
			std::vector<int> indexes = {}; // здесь будут храниться индексы подсписков, в которых присутствует заданный узел
			Node<Type>* tmp = current_head->next_node; // первый действительный узел у основного списка 
			size_t count_head{};
			while (tmp != nullptr) {
				SubNode<Type>* tmp_sub = tmp->head.next_node; // Получаем первый действительный узел у подсписка
				while (tmp_sub != nullptr) {
					if (tmp_sub->value == found_node) {
						indexes.push_back(static_cast<int>(count_head));
					}
					tmp_sub = tmp_sub->next_node;
				}
				tmp = tmp->next_node;
				++count_head;
			}
			if (indexes.size() == 0) {
				throw std::invalid_argument("Результатов нет.");
			}
			else {
				return indexes;
			}
		}
	}

	OperationStatus SelectPushType() const {
		std::cout << "Укажите позицию вставки:"s << std::endl;
		std::cout << "1 >> В конец"s << std::endl;
		std::cout << "2 >> Перед заданным значением"s << std::endl;
		std::cout << "3 >> После заданного значения"s << std::endl;
		do {
			std::string tmp;
			std::getline(std::cin, tmp);
			if (isOnlyNumbers(tmp, '1', '3')) {
				return static_cast<OperationStatus>(std::stoi(tmp));
			}
		} while (true);
	}

	void Print(Node<Type>* current_head) const {
		std::cout << "СОСТОЯНИЕ КОНТЕЙНЕРА:" << std::endl;
		if (IsEmpty(current_head)) {
			std::cout << "пусто." << std::endl;
			return;
		}
		else {
			// Получаем первый действительный узел у основного списка 
			Node<Type>* tmp = current_head->next_node;
			while (tmp != nullptr) {
				std::cout << "[" << tmp->value << "]"s << std::endl << "   "s;

				if (IsEmpty(&tmp->head)) {
					std::cout << "пусто"s;
				}
				else {
					// Получаем первый действительный узел у подсписка
					SubNode<Type>* tmp_sub = tmp->head.next_node;
					bool isFirst = true;
					while (tmp_sub != nullptr) {
						if (isFirst) {
							std::cout << tmp_sub->value;
							tmp_sub = tmp_sub->next_node;
							isFirst = false;
						}
						else {
							std::cout << ", "s << tmp_sub->value;
							tmp_sub = tmp_sub->next_node;
						}
					}
				}
				tmp = tmp->next_node;
				std::cout << ". "s << std::endl;
			}
		}
	}

	void Menu() {
		std::cout << "ВВЕДИТЕ НОМЕР КОМАНДЫ"s << std::endl;
		do {
			std::cout << "1 >> Поиск заданного значения"s << std::endl;
			std::cout << "2 >> Добавление"s << std::endl;
			std::cout << "3 >> Удаление"s << std::endl;
			std::cout << "4 >> Вывод на экран"s << std::endl;
			std::cout << "0 >> Завершение работы"s << std::endl;
			std::string user_input;
			std::getline(std::cin, user_input);

			if (isOnlyNumbers(user_input, '0', '5')) {
				status_ = static_cast<MenuStatus>(std::stoi(user_input));
				switch (status_) {
				case SinglyLinkedList::MenuStatus::EXIT_:
					break;

				case SinglyLinkedList::MenuStatus::PEEK_: {
					if (IsEmpty(&head_)) {
						ERROR_HINT("Невозможно выполнить поиск по пустому контейнеру.");
					}
					else {
						do {
							std::cout << "Укажите искомое значение: "s;
							std::string found_node;
							std::getline(std::cin, found_node);
							try {
								std::vector<int> indexes = IsExists(&head_, std::stoi(found_node));
								for (const auto& index : indexes) {
									std::cout << "Заданное значение найдено в подсписке с индексом "s << index << std::endl;
								}
							}
							catch (const std::exception& error) {
								HINT(error.what());
							}
							break;
						} while (true);
					}
					break;
				}

				case SinglyLinkedList::MenuStatus::APPEND_: {
					do {
						std::cout << "1 >> Вставка в новый подсписок"s << std::endl;
						std::cout << "2 >> Вставка в существующий подсписок"s << std::endl;
						std::cout << "0 >> Назад"s << std::endl;
						std::string tmp;
						getline(std::cin, tmp);

						if (isOnlyNumbers(tmp, '0', '2')) {

							// Вставка нового списка
							if (std::stoi(tmp) == 1) {
								std::cout << "Укажите количество создаваемых новых подсписков: "s;
								std::string tmp_size;
								getline(std::cin, tmp_size);
								if (isOnlyNumbers(tmp_size, '0', '9') and std::stoi(tmp_size) != 0) {

									std::vector<int> nums;
									do {
										std::cout << "Укажите добавляемые элементы: "s;
										std::string new_element;
										getline(std::cin, new_element);
										if (isOnlyNumbers(new_element, '0', '9')) {
											nums.push_back(stoi(new_element));
										}
										else {
											ERROR_HINT("Некорректный ввод.");
										}
									} while (nums.size() != std::stoi(tmp_size));

									Node<Type>* tmp = nullptr;
									Push(nums, tmp, &head_);
									break;

								}
								else {
									ERROR_HINT("Ожидается натуральное число или 0."s);
								}
							}

							// Вставка в подсписок
							else if (std::stoi(tmp) == 2) {
								std::cout << "Укажите значение списка, в который будет выполнена вставка: "s;
								std::string tmp_index;
								std::getline(std::cin, tmp_index);
								if (isOnlyNumbers(tmp_index, '0', '9')) {
									Node<Type>* t_head = Peek(&head_, std::stoi(tmp_index));
									SubNode<Type>* head = &t_head->head;
									if (head != nullptr) {
										std::cout << "Укажите количество новых значений: "s;
										std::string tmp_size;
										getline(std::cin, tmp_size);
										if (isOnlyNumbers(tmp_size, '0', '9') and std::stoi(tmp_size) != 0) {

											std::vector<int> nums;
											do {
												std::cout << "Укажите добавляемые элементы: "s;
												std::string new_element;
												getline(std::cin, new_element);
												if (isOnlyNumbers(new_element, '0', '9')) {
													nums.push_back(stoi(new_element));
												}
												else {
													ERROR_HINT("Некорректный ввод.");
												}
											} while (nums.size() != std::stoi(tmp_size));

											SubNode<Type>* tmp = nullptr;
											Push(nums, tmp, /*&head_.*/head);
											break;

										}
										else {
											ERROR_HINT("Ожидается натуральное число или 0."s);
										}
									}
								}
							}
							else {
								break;
							}
						}
						else {
							ERROR_HINT("Некорректный ввод.");
						}
					} while (true);
					break;
				}
				case SinglyLinkedList::MenuStatus::DELETE_: {
					do {
						std::cout << "1 >> Удаление значения в конкретном подсписке"s << std::endl;
						std::cout << "2 >> Удаление указанного подсписка полностью"s << std::endl;
						std::cout << "0 >> Назад"s << std::endl;
						std::string tmp;
						getline(std::cin, tmp);

						if (isOnlyNumbers(tmp, '0', '2')) {
							if (std::stoi(tmp) == 1) {
								// Удаление значения в конкретном подсписке
								do {
									std::string found_index;
									do {
										std::cout << "Укажите значение подсписка: "s;
										getline(std::cin, found_index);
										if (isOnlyNumbers(found_index, '0', '9')) {
											if (Peek(&head_, std::stoi(found_index))) {
												break;
											}
										}
										else ERROR_HINT("Некорректный ввод: ожидается натуральное число или 0.");
									} while (true);

									std::cout << "Укажите информационную часть удаляемого элемента: "s;
									std::string found_element;
									getline(std::cin, found_element);
									if (isOnlyNumbers(found_element, '0', '9')) {
										try {
											Delete(stoi(found_element), std::stoi(found_index), OperationStatus::DELETE_NOT_ALL_VALUES);
											std::cout << "Узел с заданным значением удалён."s << std::endl;
										}
										catch (const std::exception& error) {
											ERROR_HINT(error.what());
										}
										break;
									}
									else ERROR_HINT("Некорректный ввод: ожидается натуральное число или 0.");
								} while (true);
							}
							else if (std::stoi(tmp) == 2) {
								// Удаление указанного подсписка полностью
								do {
									std::cout << "Укажите значение удаляемого подсписка: "s;
									std::string found_index;
									getline(std::cin, found_index);
									if (isOnlyNumbers(found_index, '0', '9')) {
										try {
											Delete(0, std::stoi(found_index), OperationStatus::DELETE_SUBNODE);
											std::cout << "Узел с заданным значением удалён."s << std::endl;
										}
										catch (const std::exception& error) {
											ERROR_HINT(error.what());
										}
										break;
									}
									else ERROR_HINT("Некорректный ввод: ожидается натуральное число или 0.");
								} while (true);
								break;
							}
							else if (std::stoi(tmp) == 0) {
								// Назад
								break;
							}
							break;
						}
						else ERROR_HINT("Ожидается число в интервале [1, 2]."s);
					} while (true);
					break;
				}
				case SinglyLinkedList::MenuStatus::PRINT_: {
					Print(&head_);
					break;
				}

				default:
					break;
				}
			}
			else {
				ERROR_HINT("Ожидается число в интервале [0,5].");
			}
			std::cout << std::endl;
		} while (status_ != MenuStatus::EXIT_);
	}
};

int main() {
	setlocale(LC_ALL, "RUS");
	SinglyLinkedList<int> list;
	list.Menu();
	return 0;
}