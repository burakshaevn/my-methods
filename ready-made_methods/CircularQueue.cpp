#include <iostream> 
#include <string>

using namespace std::string_literals;

#define ERROR_HINT(hint) std::cout << "Ошибка. "s << hint << std::endl;
constexpr auto MAX_SIZE = 4;

struct Queue {
	size_t current_size;
	int front, back;
	int queue[MAX_SIZE]{ 0 };

	enum class MenuStatus {
		EXIT_,
		IS_EMPTY_,
		IS_FULLNESS_,
		APPEND_,
		DELETE_,
		CURRENT_STATUS_,
		WAIT_CHOISE
	} status_ = MenuStatus::WAIT_CHOISE;

	Queue() 
		: front(-1), back(-1), current_size(0)
	{}

	bool isOnlyNumbers(const std::string& string, const char limit_begin, const char limit_end) const {
		if (string.empty()) return false;
		else {
			for (const char& c : string) {
				if (c < limit_begin || c > limit_end) {
					return false;
				}
			}
			return true;

		}
	}

	bool isCorrectMenuInput(const std::string& string) {
		if (!isOnlyNumbers(string, '0', '5')) return false;
		else {
			status_ = static_cast<MenuStatus>(std::stoi(string));
			return true;
		}
	}

	bool QueueIsEmpty() const {
		return front == -1;
	}

	bool IsFull() const {
		if (front == 0 && back == MAX_SIZE - 1) {
			return true;
		}
		if (front == back + 1) {
			return true;
		}
		return false;
	}
	void Enqueue(int data) {
		if (IsFull()) {
			std::cout << "Невозможно выполнить добавление: переполнение контейнера." << std::endl;
			return;
		}
		/*Если front равен - 1,  это означает, что очередь пустая,
		и устанавливается значение front в 0*/
		if (front == -1) {
			front = 0;
		}
		/*Циклически инкрементируем значение BACK на 1.
		Когда указатель окажется в конце очереди,
		он должен переместится в ее начало*/
		back = (back + 1) % MAX_SIZE; // Гарантировано не будет переполнения
		/*Добавляем новый элемент на ту позицию,
		куда указывает BACK*/
		queue[back] = data;
		++current_size;
	}

	void Dequeue() {
		if (QueueIsEmpty()) {
			std::cout << "Невозможно произвести удаление: контейнер пуст." << std::endl;
			return;
		}
		else {
			/*Если front и back указывают на один и тот же элемент, это означает,
			что в очереди ОСТАЛСЯ ОДИН ЭЛЕМЕНТ,
			который удаляется путем установки front и back в -1*/
			if (front == back) {
				front = -1;
				back = -1;
			}
			/*Циклически инкрементируем значение FRONT на 1
			Остался только один элемент, значит, очередь сбрасывается в начальное
			cостояние после удаления последнего элемента*/
			else {
				front = (front + 1) % MAX_SIZE; // Гарантировано не будет переполнения
			}
			--current_size;
		}
	}

	void PrintQueue() const {
		std::cout << "СОСТОЯНИЕ КОНТЕЙНЕРА:" << std::endl;
		if (QueueIsEmpty()) {
			std::cout << "пусто." << std::endl;
			return;
		}
		else {
			/*Явное указание индексов.
			Происходит явная итерация по элементам очереди,
			начиная с индекса front и продвигаясь до индекса back.

			Range-for-based не подойдёт, потому что он просто выводит все элементы,
			не учитывая особенности кольцевой очереди (front и back), так как не происходит
			проверка элементов. Значит будут выводиться и удалённые/пустые ячейки. Прим: 100 -1 -1 -1.*/
			size_t i;
			bool isFirst = true;
			for (i = front; i != back; i = (i + 1) % MAX_SIZE) {
				std::cout << queue[i] << ", "s;
			}
			std::cout << queue[i] << ". "s;
		}
		std::cout << std::endl;
	}

	void Menu() {
		std::cout << "ВВЕДИТЕ НОМЕР КОМАНДЫ"s << std::endl;
		do {
			std::cout << "1 >> Проверка пустоты"s << std::endl;
			std::cout << "2 >> Проверка заполненности"s << std::endl;
			std::cout << "3 >> Добавление элемента"s << std::endl;
			std::cout << "4 >> Удаление элемента"s << std::endl;
			std::cout << "5 >> Вывод на экран"s << std::endl;
			std::cout << "0 >> Завершение работы программы"s << std::endl;
			std::string user_input;
			std::getline(std::cin, user_input);

			if (isCorrectMenuInput(user_input)) {
				switch (status_) {
				case Queue::MenuStatus::EXIT_:
					break;

				case Queue::MenuStatus::IS_EMPTY_:
					if (QueueIsEmpty()) {
						std::cout << "Контейнер пуст."s << std::endl;
					}
					else std::cout << "Контейнер не пуст."s << std::endl;
					break;

				case Queue::MenuStatus::IS_FULLNESS_:
					if (IsFull()) {
						std::cout << "Контейнер заполнен."s;
					}
					else {
						std::cout << "Контейнер не заполнен."s;
					}
					std::cout << "("s + std::to_string(current_size) + "/"s + std::to_string(MAX_SIZE) + ")."s << std::endl;
					break;

				case Queue::MenuStatus::APPEND_: {
					bool added = false;
					do {
						std::cout << "Введите добавляемый элемент (натуральное число или 0): "s;
						std::string n_string = ""s;
						getline(std::cin, n_string);
						if (isOnlyNumbers(n_string, '0', '9')) {
							Enqueue(stoi(n_string));
							added = true;
						}
						else { ERROR_HINT("Ожидается натуральное число или 0."); std::cout << std::endl; }
					} while (added != true);
					break;
				}

				case Queue::MenuStatus::DELETE_: {
					bool deleted = false;
					do {
						Dequeue();
						deleted = true;
					} while (deleted != true);
					break;
				}

				case Queue::MenuStatus::CURRENT_STATUS_:
					PrintQueue();
					break;

				default:
					break;
				}
			}
			else ERROR_HINT("Ожидается число в интервале [0,5].");
			std::cout << std::endl;
		} while (status_ != MenuStatus::EXIT_);
	}
};

int main() {
	setlocale(LC_ALL, "RUS");
	Queue queue;
	queue.Menu();
	return 0;
}