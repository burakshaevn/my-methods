#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class StackVector {
public:

    // Конструктор принимает размер вектора — по аналогии со стандартным вектором. 
    // Если аргумент конструктора больше вместимости вектора, конструктор должен выбрасывать исключение invalid_argument.
    explicit StackVector(size_t a_size = 0)
        : size_(a_size) {
        if (size_ > Capacity()) {
            throw std::invalid_argument("size_ > capacity");
        }
    }

    T& operator[](size_t index) { 
        return stack_vector.at(index);
    }

    const T& operator[](size_t index) const {
        return stack_vector[index];
    }

    // Методы begin/end должны возвращать итераторы на начало и текущий конец вектора. Их тип не указан, выберите его самостоятельно.
    typename std::array<T, N>::iterator begin() {
        return stack_vector.begin();
    }
    typename std::array<T, N>::iterator end() {
        return stack_vector.begin() + size_;
    }
    typename std::array<T, N>::const_iterator begin() const {
        return stack_vector.begin();
    }
    typename std::array<T, N>::const_iterator end() const {
        return stack_vector.begin() + size_;
    }

    // Метод Size должен возвращать текущее количество объектов в векторе.
    size_t Size() const {
        return size_;
    }

    // Метод Capacity должен возвращать вместимость вектора — то есть количество объектов, которое в него может поместиться.
    size_t Capacity() const {
        return stack_vector.size();
    }

    // Метод PushBack добавляет новый элемент в конец вектора. Если текущий размер вектора равен его вместимости, метод PushBack должен выбрасывать стандартное исключение overflow_error.
    void PushBack(const T& value) {
        if (Size() == Capacity()) {
            throw std::overflow_error("Size = Capacity");
        }
        else {
            stack_vector[size_] = value;
            ++size_;
        }
    }

    // Метод PopBack уменьшает размер вектора на один и возвращает элемент вектора, который был последним. Если вектор пуст, метод PopBack должен выбрасывать стандартное исключение underflow_error.
    T PopBack() {
        if (Size() == 0) {
            throw std::underflow_error("Vector is empty");
        }
        else {
            --size_;
            return stack_vector[size_];
        }
    }

private:
    std::array<T, N> stack_vector;
    size_t size_;
};
