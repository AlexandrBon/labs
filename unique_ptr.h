#pragma once
#include <type_traits>
#include <utility>
namespace lab_05::unique {
template <typename T>
struct deleter {
    void operator()(T *ptr) {
        delete ptr;
    }
};
template <typename T>
struct deleter<T[]> {
    void operator()(T *ptr) {
        delete[] ptr;
    }
};
template <typename T, typename Deleter = deleter<T>>
struct unique_ptr {
private:
    std::remove_extent_t<T> *data_;
    Deleter deleter_;

public:
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr &operator=(const unique_ptr &) = delete;
    explicit unique_ptr(std::remove_extent_t<T> *arr = nullptr) : data_(arr) {
    }
    explicit unique_ptr(std::remove_extent_t<T> *arr, const Deleter &arr_del)
        : data_(arr), deleter_(arr_del) {
    }
    explicit unique_ptr(std::remove_extent_t<T> *arr, Deleter &&arr_del)
        : data_(arr), deleter_(std::move(arr_del)) {
    }
    explicit unique_ptr(unique_ptr &&move)
        : data_(std::exchange(move.data_, nullptr)) {
    }
    ~unique_ptr() {
        deleter_(data_);
    }
    explicit operator bool() const {
        return !(data_ == nullptr);
    }
    void reset(std::remove_extent_t<T> *data_ = nullptr) {
        deleter_(this->data_);
        this->data_ = data_;
    }
    unique_ptr &operator=(unique_ptr &&move) {
        if (this->data_ != move.data_) {
            deleter_(this->data_);
        }
        this->data_ = std::exchange(move.data_, nullptr);
        return *this;
    }
    const std::remove_extent_t<T> *get() const {
        return data_;
    }
    std::remove_extent_t<T> *operator->() const {
        return data_;
    }
    std::remove_extent_t<T> &operator*() const {
        return *data_;
    }
    std::remove_extent_t<T> &operator[](std::size_t in) const {
        return *(data_ + in);
    }
    const std::remove_extent_t<T> *release() {
        return std::exchange(data_, nullptr);
    }
    void swap(unique_ptr &other) {
        using std::swap;
        swap(this->data_, other.data_);
    }
};
template <typename T, typename Deleter>
void swap(unique_ptr<T, Deleter> &lptr, unique_ptr<T, Deleter> &rptr) {
    lptr.swap(rptr);
}
template <typename T, typename Deleter>
bool operator==(const unique_ptr<T, Deleter> &lptr,
                const unique_ptr<T, Deleter> &rptr) {
    return lptr.get() == rptr.get();
}
template <typename T, typename Deleter>
bool operator!=(const unique_ptr<T, Deleter> &lptr,
                const unique_ptr<T, Deleter> &rptr) {
    return lptr.get() != rptr.get();
}
}  // namespace lab_05::unique
