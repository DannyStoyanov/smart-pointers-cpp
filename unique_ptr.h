#pragma once
#include <iostream>
#include <memory>

// Single object deleter version
template <typename T>
struct default_deleter { // functor or function object
	void operator()(T* ptr) {
		if(ptr!=nullptr) {
			// std::cout<<"dafault_deleter"<<std::endl;
			// Direct delete on the pointer
			// Here cannot be any custom cleanup
			delete ptr;
		}
	}
};

// Single object specialization
template<class T, class Deleter = default_deleter<T>>
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

	// std::unique_ptr<T, Deleter>::operator=
	unique_ptr<T, Deleter>(unique_ptr<T, Deleter>&& other);
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
template <class T, class Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr() noexcept: ptr(nullptr), del(default_deleter<T>()) {}

template <class T, class Deleter>
constexpr unique_ptr<T, Deleter>::unique_ptr(std::nullptr_t) noexcept: unique_ptr() {}

template <class T, class Deleter>
unique_ptr<T, Deleter>::unique_ptr(T* ptr) noexcept: ptr(ptr), del(default_deleter<T>()) {}

template <class T, class Deleter>
unique_ptr<T, Deleter>::unique_ptr(T* ptr, Deleter del) noexcept: ptr(ptr), del(del) {}

// std::unique_ptr<T, Deleter>::operator=
template <class T, class Deleter>
unique_ptr<T, Deleter>::unique_ptr(unique_ptr<T, Deleter>&& other) {
	this->ptr = other.ptr;
	other.ptr = nullptr;
	del = other.del;
}

template <class T, class Deleter>
unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(std::nullptr_t) noexcept {
	reset();
}

template <class T, class Deleter>
unique_ptr<T, Deleter>& unique_ptr<T, Deleter>::operator=(const unique_ptr<T, Deleter>&& ptr) noexcept {
	if(*this != &ptr) {
		this->swap(ptr);
	}
	return *this;
}

// Destructor:
template <class T, class Deleter>
unique_ptr<T, Deleter>::~unique_ptr() {
	if(!(get() == nullptr)) {
		get_deleter()(get());
	}
}

// Modifiers:
template <class T, class Deleter>
T* unique_ptr<T, Deleter>::release() noexcept {
	T* temp = get();
	this->ptr = nullptr;
	return temp;
}

template <class T, class Deleter>
void unique_ptr<T, Deleter>::reset(T* ptr) noexcept {
	T* old_ptr = this->ptr;
	this->ptr = ptr;
	if(old_ptr) {
		get_deleter()(old_ptr);
	}
}

template <class T, class Deleter>
void unique_ptr<T, Deleter>::swap(unique_ptr<T, Deleter>& other) noexcept {
	std::swap(this->ptr, other.ptr);
	std::swap(this->del, other.del);
}

// Observers:
template <class T, class Deleter>
T* unique_ptr<T, Deleter>::get() const noexcept {
	return this->ptr;
}

template <class T, class Deleter>
Deleter& unique_ptr<T, Deleter>::get_deleter() noexcept {
	return this->del;
}

template <class T, class Deleter>
unique_ptr<T, Deleter>::operator bool() const noexcept {
	return get() != nullptr;
}

// Operators:

// Member operators:
template <class T, class Deleter>
T* unique_ptr<T, Deleter>::operator->() const noexcept {
	return get();
}

template <class T, class Deleter>
T unique_ptr<T, Deleter>::operator*() const noexcept {
	return *get();
}

// Non-member operators:
template <class T1, class Deleter1, class T2, class Deleter2>
bool operator==(const unique_ptr<T1, Deleter1>& x, const unique_ptr<T2, Deleter2>& y) {
	return x.get() == y.get();
}

template <class T, class Deleter>
bool operator==(const unique_ptr<T, Deleter>& x, std::nullptr_t) noexcept {
	return !x;
}

template <class T, class Deleter>
bool operator==(std::nullptr_t, const unique_ptr<T, Deleter>& x) noexcept {
	return !x;
}

template <class T1, class Deleter1, class T2, class Deleter2>
bool operator!=(const unique_ptr<T1, Deleter1>& x, const unique_ptr<T2, Deleter2>& y) {
	return x.get != y.get();
}

template <class T, class Deleter>
bool operator!=(const unique_ptr<T, Deleter>& x, std::nullptr_t) noexcept {
	return (bool)x;
}

template <class T, class Deleter>
bool operator!=(std::nullptr_t, const unique_ptr<T, Deleter>& x) noexcept {
	return (bool)x;
}

// Non-member functions:

// std::swap
template< class T, class Deleter>
void swap( std::unique_ptr<T, Deleter>& lhs, std::unique_ptr<T, Deleter>& rhs ) noexcept {
	lhs.swap(rhs);
}

// make_unique<T>
template< class T, class... Args >
unique_ptr<T> make_unique( Args&&... args ) {
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Array deleter version
template <typename T>
struct default_deleter<T[]> { // functor or function object
	void operator()(T* ptr) {
		if(ptr!=nullptr) {
			// std::cout<<"dafault_deleter []"<<std::endl;
			// Direct delete on the pointer
			// Here cannot be any custom cleanup
			delete[] ptr;
		}
	}
};

// Array specialization
template<class T, class Deleter>
class unique_ptr<T[], Deleter> {
	private:
		T* ptr;
		Deleter del;
	public:
		// Constructors:
		constexpr unique_ptr() noexcept;
		constexpr unique_ptr(std::nullptr_t) noexcept;
		explicit unique_ptr(T* ptr) noexcept;
		unique_ptr(T* ptr, Deleter del) noexcept;

		// std::unique_ptr<T[], Deleter>::operator=
		unique_ptr<T[], Deleter>(unique_ptr<T[], Deleter>&& other);
		unique_ptr<T[], Deleter>& operator= (std::nullptr_t) noexcept;
		unique_ptr<T[], Deleter>& operator=(const unique_ptr<T[], Deleter>&& ptr) noexcept;

		// Destructor:
		~unique_ptr();

		// Remove compiler generated copy semantics:
		unique_ptr(const unique_ptr<T[], Deleter>& ptr) = delete;
		unique_ptr<T[], Deleter>& operator=(const unique_ptr<T[], Deleter>& ptr) = delete;

		// Modifiers:
		T* release() noexcept;
		void reset(T* ptr) noexcept;
		void swap(unique_ptr<T[], Deleter>& other) noexcept;

		// Observers:
		T* get() const noexcept;
		Deleter& get_deleter() noexcept;
		explicit operator bool() const noexcept;

		// Operators:
		T& operator[](std::size_t i) const;
};

// Constructors:
template <class T, class Deleter>
constexpr unique_ptr<T[], Deleter>::unique_ptr() noexcept: ptr(nullptr), del(default_deleter<T[]>()) {}

template <class T, class Deleter>
constexpr unique_ptr<T[], Deleter>::unique_ptr(std::nullptr_t) noexcept: unique_ptr() {}

template <class T, class Deleter>
unique_ptr<T[], Deleter>::unique_ptr(T* ptr) noexcept: ptr(ptr), del(default_deleter<T[]>()) {}

template <class T, class Deleter>
unique_ptr<T[], Deleter>::unique_ptr(T* ptr, Deleter del) noexcept: ptr(ptr), del(del) {}

// std::unique_ptr<T[], Deleter>::operator=
template <class T, class Deleter>
unique_ptr<T[], Deleter>::unique_ptr(unique_ptr<T[], Deleter>&& other) {
	this->ptr = other.ptr;
	other.ptr = nullptr;
	del = other.del;
}

template <class T, class Deleter>
unique_ptr<T[], Deleter>& unique_ptr<T[], Deleter>::operator= (std::nullptr_t) noexcept {
	reset();
}

template <class T, class Deleter>
unique_ptr<T[], Deleter>& unique_ptr<T[], Deleter>::operator=(const unique_ptr<T[], Deleter>&& ptr) noexcept {
	if(*this != &ptr) {
		this->swap(ptr);
	}
	return *this;
}

// Destructor:
template <class T, class Deleter>
unique_ptr<T[], Deleter>::~unique_ptr() {
	if(!(get() == nullptr)) {
		get_deleter()(get());
	}
}

// Modifiers:
template <class T, class Deleter>
T* unique_ptr<T[], Deleter>::release() noexcept {
	T* temp = get();
	this->ptr = nullptr;
	return temp;
}

template <class T, class Deleter>
void unique_ptr<T[], Deleter>::reset(T* ptr) noexcept {
	T* old_ptr = this->ptr;
	this->ptr = ptr;
	if(old_ptr) {
		get_deleter()(old_ptr);
	}
}

template <class T, class Deleter>
void unique_ptr<T[], Deleter>::swap(unique_ptr<T[], Deleter>& other) noexcept {
	std::swap(this->ptr, other.ptr);
	std::swap(this->del, other.del);
}

// Observers:
template <class T, class Deleter>
T* unique_ptr<T[], Deleter>::get() const noexcept {
	return this->ptr;
}

template <class T, class Deleter>
Deleter& unique_ptr<T[], Deleter>::get_deleter() noexcept {
	return this->del;
}

template <class T, class Deleter>
unique_ptr<T[], Deleter>::operator bool() const noexcept {
	return get() != nullptr;
}

// Operators:

// Member operators:
template <class T, class Deleter>
T& unique_ptr<T[], Deleter>::operator[](std::size_t i) const {
	return get()[i];
}

// Non-member operators:
template <class T1, class Deleter1, class T2, class Deleter2>
bool operator==(const unique_ptr<T1[], Deleter1>& x, const unique_ptr<T2[], Deleter2>& y) {
	return x.get() == y.get();
}

template <class T, class Deleter>
bool operator==(const unique_ptr<T[], Deleter>& x, std::nullptr_t) noexcept {
	return !x;
}

template <class T, class Deleter>
bool operator==(std::nullptr_t, const unique_ptr<T[], Deleter>& x) noexcept {
	return !x;
}

template <class T1, class Deleter1, class T2, class Deleter2>
bool operator!=(const unique_ptr<T1[], Deleter1>& x, const unique_ptr<T2[], Deleter2>& y) {
	return x.get != y.get();
}

template <class T, class Deleter>
bool operator!=(const unique_ptr<T[], Deleter>& x, std::nullptr_t) noexcept {
	return (bool)x;
}

template <class T, class Deleter>
bool operator!=(std::nullptr_t, const unique_ptr<T[], Deleter>& x) noexcept {
	return (bool)x;
}