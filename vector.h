#ifndef VECTOR_H_
#define VECTOR_H_

#include <cstddef>
#include <exception>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace lab_07 {

template <typename T>
void destroy(T *data, std::size_t l, std::size_t r) noexcept {
    for (std::size_t i = l; i < r; i++) {
        data[i].~T();
    }
}

template <typename T, typename Alloc = std::allocator<T>>
struct Memory {
private:
    std::size_t capacity = 0;
    T *data = nullptr;

public:
    static std::size_t calc_capacity(std::size_t n) noexcept {
        if (n == 0) {
            return 0;
        }
        --n;
        std::size_t k = 0;
        while (n > 0) {
            n >>= 1;
            k++;
        }
        return 1 << k;
    }

    [[nodiscard]] const T *get_data() const &noexcept {
        return this->data;  // cppcheck-suppress [CastIntegerToAddressAtReturn]
    }

    [[nodiscard]] std::size_t get_capacity() const &noexcept {
        return capacity;
    }

    explicit Memory(std::size_t n) noexcept : capacity(calc_capacity(n)) {
        if (capacity != 0) {
            data = Alloc().allocate(capacity);
        }
    }

    Memory() = default;

    Memory(Memory &&other) noexcept {
        swap(other);
    }

    Memory &operator=(Memory &) = delete;
    Memory(Memory &) = delete;

    Memory &operator=(Memory &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        swap(other);
        return *this;
    }

    ~Memory() {
        if (capacity != 0) {
            Alloc().deallocate(data, capacity);
        }
    }

    T &operator[](const std::size_t id) &noexcept {
        return data[id];
    }

    T &&operator[](const std::size_t id) &&noexcept {
        return std::move(data[id]);
    }

    const T &operator[](const std::size_t id) const &noexcept {
        return data[id];
    }

    T *operator+(const std::size_t id) noexcept {
        return data + id;
    }

    const T *operator+(const std::size_t id) const noexcept {
        return data + id;
    }

    void swap(Memory &other) noexcept {
        using std::swap;
        swap(data, other.data);
        swap(capacity, other.capacity);
    }
};

template <typename T, typename Alloc = std::allocator<T>>
class vector {
    Memory<T, Alloc> data;
    std::size_t sz = 0;

    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_move_assignable_v<T>);
    static_assert(std::is_nothrow_destructible_v<T>);

public:
    [[nodiscard]] std::size_t size() const noexcept {
        return sz;
    }

    [[nodiscard]] std::size_t capacity() const noexcept {
        return data.get_capacity();
    }

    [[nodiscard]] bool empty() const noexcept {
        return sz == 0;
    }

    T &operator[](const std::size_t id) &noexcept {
        return data[id];
    }

    T &&operator[](const std::size_t id) &&noexcept {
        return std::move(data[id]);
    }

    const T &operator[](const std::size_t id) const &noexcept {
        return data[id];
    }

    T &at(const std::size_t id) & {
        if (id < sz) {
            return data[id];
        } else {
            throw std::out_of_range("error");
        }
    }

    T &&at(const std::size_t id) && {
        if (id < sz) {
            return std::move(data[id]);
        } else {
            throw std::out_of_range("error");
        }
    }

    const T &at(const std::size_t id) const & {
        if (id < sz) {
            return data[id];
        } else {
            throw std::out_of_range("error");
        }
    }

    explicit vector(std::size_t n) : data(n) {
        for (std::size_t i = 0; i < n; i++) {
            try {
                new (data + i) T();
            } catch (...) {
                destroy(data.get_data(), 0, i);
                throw;
            }
        }
        sz = n;
    }

    vector(const vector &other) : data(other.sz) {
        for (std::size_t i = 0; i < other.sz; i++) {
            try {
                new (data + i) T(other.data[i]);
            } catch (...) {
                destroy(data.get_data(), 0, i);
                throw;
            }
        }
        sz = other.sz;
    }

    vector(std::size_t n, const T &value) : data(n) {
        for (std::size_t i = 0; i < n; i++) {
            try {
                new (data + i) T(value);
            } catch (...) {
                destroy(data.get_data(), 0, i);
                throw;
            }
        }
        sz = n;
    }

    vector(vector &&other) noexcept {
        swap(other);
    }

    vector &operator=(vector &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        swap(other);
        destroy(other.data.get_data(), 0, other.sz);
        other.sz = 0;
        return *this;
    }

    vector &operator=(const vector &other) {
        if (&other == this) {
            return *this;
        }
        vector tmp(other);
        swap(tmp);
        return *this;
    }

    vector() = default;

    ~vector() {
        destroy(data.get_data(), 0, sz);
    }

    void swap(vector &other) noexcept {
        data.swap(other.data);
        std::swap(sz, other.sz);
    }

    void reserve(std::size_t n) noexcept {
        std::size_t new_capacity = Memory<T>::calc_capacity(n);
        if (data.get_capacity() < new_capacity) {
            Memory<T> data_new(n);
            for (std::size_t i = 0; i < sz; i++) {
                new (data_new + i) T(std::move(data[i]));
            }
            destroy(data.get_data(), 0, sz);
            data.swap(data_new);
        }
    }

    void resize(const std::size_t n) {
        if (sz < n) {
            for (std::size_t i = sz; i < n; i++) {
                try {
                    this->push_back(T());
                } catch (...) {
                    vector old_vector;
                    for (std::size_t j = 0; j < sz; j++) {
                        old_vector.push_back(std::move(this->data[j]));
                    }
                    *this = std::move(old_vector);
                    throw;
                }
            }
        } else if (sz > n) {
            destroy(data.get_data(), n, sz);
        }
        sz = n;
    }

    void resize(const std::size_t n, const T &value) {
        if (sz < n) {
            for (std::size_t i = sz; i < n; i++) {
                try {
                    this->push_back(value);
                } catch (...) {
                    vector old_vector;
                    for (std::size_t j = 0; j < sz; j++) {
                        old_vector.push_back(std::move(this->data[j]));
                    }
                    *this = std::move(old_vector);
                    throw;
                }
            }
        } else if (sz > n) {
            destroy(data.get_data(), n, sz);
        }
        sz = n;
    }

    void push_back(const T &value) {
        if (sz == data.get_capacity()) {
            vector new_vector(*this);
            new_vector.reserve(sz == 0 ? 1 : sz << 1);
            try {
                new (new_vector.data + sz) T(value);
            } catch (...) {
                throw;
            }
            *this = std::move(new_vector);
        } else {
            try {
                new (data + sz) T(value);
            } catch (...) {
                throw;
            }
        }
        ++sz;
    }

    void push_back(T &&value) noexcept {
        if (sz == data.get_capacity()) {
            reserve(sz == 0 ? 1 : sz << 1);
        }
        new (data + sz) T(std::move(value));
        ++sz;
    }

    void pop_back() noexcept {
        data[--sz].~T();
    }

    void clear() noexcept {
        destroy(data.get_data(), 0, sz);
        sz = 0;
    }
};
}  // namespace lab_07

#endif  // VECTOR_H_
