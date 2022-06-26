//Sprint 6. SingleLinkedList v0.1.1.
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& value, Node* next)
			: value(value)
			, next_node(next) {
		}
		Type value;
		Node* next_node = nullptr;
	};
	// Шаблон класса «Базовый Итератор».
	// Определяет поведение итератора на элементы односвязного списка
	// ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
	template <typename ValueType>
	class BasicIterator {
		// Класс списка объявляется дружественным, чтобы из методов списка
		// был доступ к приватной области итератора
		friend class SingleLinkedList;
		// Конвертирующий конструктор итератора из указателя на узел списка
		explicit BasicIterator(Node* node)
			:node_(node) {
		}

	public:
		// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
		// Категория итератора — forward iterator
		// (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;

		BasicIterator() = default;
		// Конвертирующий конструктор/конструктор копирования
		// При ValueType, совпадающем с Type, играет роль копирующего конструктора
		// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
		BasicIterator(const BasicIterator<Type>& other) noexcept
			:node_(other.node_){
		}
		// Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
		// пользовательского конструктора копирования, явно объявим оператор = и
		// попросим компилятор сгенерировать его за нас
		BasicIterator& operator=(const BasicIterator& rhs) = default;
		// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}
		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return this->node_ != rhs.node_;
		}
		// Оператор сравнения итераторов (в роли второго аргумента итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}
		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return this->node_ != rhs.node_;
		}
		// Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает ссылку на самого себя
		// Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
		BasicIterator& operator++() noexcept {
			assert(this->node_ != nullptr);
			node_ = node_->next_node ? node_->next_node : nullptr;
			return *this;
		}
		// Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает прежнее значение итератора
		// Инкремент итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		BasicIterator operator++(int) noexcept {
			auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
			++(*this); // используем логику префиксной формы инкремента
			return old_value;
		}
		BasicIterator& operator+=(size_t count) noexcept {
			for (size_t i = 0; i < count; ++i) {
				//Тут assert не нужен. При выходе за границу теперь словим assert в методе BasicIterator& operator++()
				//assert(this->node_ != nullptr);
				++(*this);
			}
			return *this;
		}

		BasicIterator& operator+(size_t count) noexcept {
			auto it(*this);
			return it+=count;
		}
		// Операция разыменования. Возвращает ссылку на текущий элемент
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] reference operator*() const noexcept {
			assert(this->node_ != nullptr);
			ValueType& result = node_->value;
			return result;
		}
		// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] pointer operator->() const noexcept {
			assert(this->node_ != nullptr);
			ValueType& result = node_->value;
			return &result;
		}

	private:
		Node* node_ = nullptr;
	};

public:
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	// Итератор, допускающий изменение элементов списка
	using Iterator = BasicIterator<Type>;
	// Константный итератор, предоставляющий доступ для чтения к элементам списка
	using ConstIterator = BasicIterator<const Type>;
	//Конструктор по-умолчанию
	SingleLinkedList()
		: head_(), size_(0) {
	}

	SingleLinkedList(const SingleLinkedList& other)
		: head_(), size_(0) {
		SingleLinkedList<Type> tmp;
		auto pos_it = tmp.before_begin();
		for(auto value_it = other.begin(); value_it != other.end(); ++value_it) {
			tmp.InsertAfter(pos_it,*value_it);
			++pos_it;
		}
		swap(tmp);
	}

	SingleLinkedList(std::initializer_list<Type> values)
		: head_(), size_(0) {
		SingleLinkedList<Type> tmp;
		for (auto it = rbegin(values); it != rend(values); ++it) {
			tmp.PushFront(*it);
		}
		swap(tmp);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (this->begin() == rhs.begin()) {
			return *this;
		}
		SingleLinkedList<Type> tmp(rhs);
		if (this->size_ != 0) {
			this->Clear();
		}
		this->swap(tmp);
		return *this;
	}

	void Clear() {
		while (head_.next_node) {
			PopFront();
		}
	}

	//Удаляет элемент, следующий за pos.
	// Возвращает итератор на элемент, следующий за удалённым
	Iterator EraseAfter(ConstIterator pos) noexcept {
		if(pos == end() || !pos.node_->next_node) {
			return Iterator(nullptr);
		}
		auto erase_node = pos.node_->next_node;
		pos.node_->next_node = erase_node->next_node;
		delete erase_node;
		--size_;
		return Iterator(pos.node_->next_node);
	}
	// Возвращает количество элементов в списке за время O(1)
	size_t GetSize() const noexcept {
		return size_;
	}
	/* Вставляет элемент value после элемента, на который указывает pos.
	 * Возвращает итератор на вставленный элемент
	 * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии */
	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		if (pos == end()) {
			return Iterator(nullptr);
		}
		Node* insert_element = new Node(value, pos.node_->next_node);
		pos.node_->next_node = insert_element;
		++size_;
		return Iterator(insert_element);
	}
	// Сообщает, пустой ли список за время O(1)
	bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	void PopFront () {
		if(!head_.next_node) {
			return;
		}
		Node* front_node = head_.next_node;
		head_.next_node = head_.next_node->next_node;
		delete front_node;
		--size_;
	}

	void PushFront(const Type& value) {
		Node* front_element = new Node(value, head_.next_node);
		head_.next_node = front_element;
		++size_;
	}

	// Обменивает содержимое списков за время O(1)
	void swap(SingleLinkedList& other) noexcept {
		std::swap(size_, other.size_);
		std::swap(head_.next_node, other.head_.next_node);
	}
	// Возвращает итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	[[nodiscard]] Iterator begin() noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}
	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator end() noexcept {
		return BasicIterator<Type>();;
	}
	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	// Результат вызова эквивалентен вызову метода cbegin()
	[[nodiscard]] ConstIterator begin() const noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}
	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	// Результат вызова эквивалентен вызову метода cend()
	[[nodiscard]] ConstIterator end() const noexcept {
		return BasicIterator<Type>();
	}
	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен cend()
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}
	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cend() const noexcept {
		return BasicIterator<Type>();
	}
	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator before_begin() noexcept {
		return BasicIterator<Type>(&head_);
	}
	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return BasicIterator<Type>(const_cast<Node*>(&head_));

	}
	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return cbefore_begin();
	}

	~SingleLinkedList(){
		Clear();
	}
private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return lhs.GetSize() == rhs.GetSize() ?
			std::equal(lhs.begin(), lhs.end(), rhs.begin())
			: false;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	// Заглушка. Реализуйте сравнение самостоятельно
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs > rhs);
}
