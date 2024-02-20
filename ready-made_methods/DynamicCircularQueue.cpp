#include <iostream> 
#include <string>

using namespace std::string_literals;

#define ERROR_HINT(hint) std::cout << "Ошибка. "s << hint << std::endl;

struct QueueElement {
	int data_;
	QueueElement* next_;

	QueueElement() : data_(0), next_(nullptr)
	{}
	QueueElement(int data) : data_(data), next_(new QueueElement())
	{}
};

struct Queue {
	size_t size_ = 0;
	QueueElement* front = nullptr;
	QueueElement* back = nullptr;

	enum class MenuStatus {
		EXIT_,
		IS_EMPTY_,
		APPEND_,
		DELETE_,
		CURRENT_STATUS_,
		WAIT_CHOISE
	};
	MenuStatus status_ = MenuStatus::WAIT_CHOISE;

	Queue() {
		front = new QueueElement(0);
		front->next_ = nullptr;
		back = front;
	}

	~Queue() {
		while (front->next_ != nullptr) {
			this->Delete();
		}
	}

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
		return front == nullptr || front->next_ == nullptr;
	}

	void Append(int data) {
		QueueElement* temp = new QueueElement(data);
		temp->next_ = nullptr;
		back->next_ = temp;
		back = temp;
	}

	void Delete() {
		if (QueueIsEmpty()) {
			ERROR_HINT("Невозможно произвести удаление: очередь пуста."s);
			return;
		}
		else {
			QueueElement* temp = front;
			if (front == back) {
				front = nullptr;
				back = nullptr;
			}
			else {
				front = front->next_;
			}
			delete temp;
		}
	}

	void PrintQueue() {
		std::cout << "СОСТОЯНИЕ ОЧЕРЕДИ:"s << std::endl;
		if (QueueIsEmpty()) {
			std::cout << "Очередь пуста."s << std::endl;
		}
		else {
			QueueElement* temp = front->next_;
			while (temp != nullptr) {
				std::cout << temp->data_ << " "s;
				temp = temp->next_;
			}
		}
		std::cout << std::endl;
	}

	void Menu() {
		std::cout << "ВВЕДИТЕ НОМЕР КОМАНДЫ"s << std::endl;
		do {
			std::cout << "1 >> Проверка пустоты очереди"s << std::endl;
			std::cout << "2 >> Добавление элемента в вершину очереди"s << std::endl;
			std::cout << "3 >> Удаление элемента из вершины очереди"s << std::endl;
			std::cout << "4 >> Вывод текущего состояние очереди на экран"s << std::endl;
			std::cout << "0 >> Завершение работы программы"s << std::endl;
			std::string user_input;
			std::getline(std::cin, user_input);

			if (isCorrectMenuInput(user_input)) {
				switch (status_) {
				case Queue::MenuStatus::EXIT_:
					break;

				case Queue::MenuStatus::IS_EMPTY_:
					if (QueueIsEmpty()) {
						std::cout << "Очередь пуста."s << std::endl;
					}
					else std::cout << "Очередь не пуста."s << std::endl;
					break;

				case Queue::MenuStatus::APPEND_: {
					bool added = false;
					do {
						std::cout << "Введите добавляемый элемент (натуральное число или 0): "s;
						std::string n_string = ""s;
						getline(std::cin, n_string);
						if (isOnlyNumbers(n_string, '0', '9')) {
							Append(stoi(n_string));
							added = true;
							std::cout << "Элемент добавлен."s << std::endl;
						}
						else { ERROR_HINT("Ожидается число в интервале число [1, inf)."); std::cout << std::endl; }
					} while (added != true);
					break;
				}

				case Queue::MenuStatus::DELETE_: {
					bool deleted = false;
					do {
						Delete();
						deleted = true;
						std::cout << "Элемент удалён."s << std::endl;
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