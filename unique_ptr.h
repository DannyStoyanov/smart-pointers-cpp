#pragma once
#include <iostream>
#include <memory>

template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
private:
	T* ptr;
	Deleter del;
public:
	// Constructors:
	constexpr unique_ptr() noexcept;
	constexpr unique_ptr(std::nullptr_t) noexcept;
	explicit unique_ptr(T* ptr) noexcept;
	unique_ptr(T* ptr, Deleter del) noexcept;

	// std::unique_ptr<T,Deleter>::operator=
	unique_ptr<T, Deleter>& operator= (std::nullptr_t) noexcept;
	unique_ptr<T, Deleter>& operator=(const unique_ptr<T, Deleter>&& ptr) noexcept;

	// Destructor:
	~unique_ptr();

	// Remove compiler generated copy semantics:
	unique_ptr(const unique_ptr<T, Deleter>& ptr) = delete;
	unique_ptr<T, Deleter>& operator=(const unique_ptr<T, Deleter>& ptr) = delete;

	// Modifiers:
	T* release() noexcept;
	void reset(T* ptr) noexcept;
	void swap(unique_ptr<T, Deleter>& other) noexcept;

	// Observers:
	T* get() const noexcept;
	Deleter& get_deleter() noexcept;
	explicit operator bool() const noexcept;
	
	// Operators:
	T* operator->() const noexcept;
	T operator*() const noexcept;
	
};

// Constructors:
template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr() noexcept {
	this->ptr = nullptr;
	std::default_delete<T> obj;
	this->del = obj; //std::default_delete<T>::default_delete()
}

template <typename T, typename Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(std::nullptr_t) noexcept: unique_ptr() {};

template <typename T, typename Deleter>
unique_ptr<T, Deleter>::unique_ptr(T* ptr) noexcept {
	this->ptr = ptr;
	std::default_delete<T> obj;
	this->del = obj; //std::default_delete<T>::default_delete()
}

template <typename T, typename Deleter>
unique_ptr<T, Deleter>::unique_ptr(T* ptr, Deleter del) noexcept {
	this->ptr = ptr;
	this->del = del;
}

// std::unique_ptr<T,Deleter>::operator=
template <typename T, typename Deleter>
unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator= (std::nullptr_t) noexcept {
	reset();
}

template <typename T, typename Deleter>
unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(const unique_ptr<T, Deleter>&& ptr) noexcept {
	if(*this != &ptr) {
		this->swap(ptr);
	}
	return *this;
}

// Destructor:
template <typename T, typename Deleter>
unique_ptr<T, Deleter>::~unique_ptr() {
	if(!(get() == nullptr)) {
		get_deleter()(get());
	}

}
// Modifiers:
template <typename T, typename Deleter>
T* unique_ptr<T, Deleter>::release() noexcept {
	T* temp = get();
	this->ptr = nullptr;
	return temp;
}

template <typename T, typename Deleter>
void unique_ptr<T, Deleter>::reset(T* ptr) noexcept {
	T* old_ptr = this->ptr;
	this->ptr = ptr;
	if(old_ptr) {
		get_deleter()(old_ptr);
	}
}

template <typename T, typename Deleter>
void unique_ptr<T, Deleter>::swap(unique_ptr<T, Deleter>& other) noexcept {
	std::swap(this->ptr, other.ptr);
	std::swap(this->del, other.del);
}

// Observers:
template <typename T, typename Deleter>
T* unique_ptr<T, Deleter>::get() const noexcept {
	return this->ptr;
}

template <typename T, typename Deleter>
Deleter& unique_ptr<T, Deleter>::get_deleter() noexcept {
	return this->del;
}

template <typename T, typename Deleter>
unique_ptr<T, Deleter>::operator bool() const noexcept {
	return get() != nullptr;
}


// Operators:
template <typename T, typename Deleter>
T* unique_ptr<T, Deleter>::operator->() const noexcept {
	return get();
}

template <typename T, typename Deleter>
T unique_ptr<T, Deleter>::operator*() const noexcept {
	return *get();
}
