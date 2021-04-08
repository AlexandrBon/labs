#pragma once
#include <type_traits>
#include <utility>

namespace lab_05::shared {
template <typename T>
struct shared_ptr {
private:
    T *data_;
    std::size_t *count_;

public:
    explicit shared_ptr(T *ptr = nullptr) : data_(ptr) {
        if (data_ == nullptr) {
            count_ = nullptr;
        } else {
            count_ = new std::size_t(1);
        }
    }
    explicit shared_ptr(const shared_ptr &other)
        : data_(other.data_), count_(other.count_) {
        if (count_ != nullptr) {
            ++(*count_);
        }
    }  // cppcheck-suppress copyCtorPointerCopying
    // move
    explicit shared_ptr(shared_ptr &&other)
        : data_(std::exchange(other.data_, nullptr)), count_(other.count_) {
        other.count_ = nullptr;
    }
    shared_ptr &operator=(const shared_ptr &other) {
        if (this->count_ != other.count_) {
            rem();
            this->data_ = other.data_;
            this->count_ = other.count_;
            if (this->count_ != nullptr) {
                ++(*this->count_);
            }
        }
        return *this;
    }
    // move
    shared_ptr &operator=(shared_ptr &&other) {
        if (this->count_ != other.count_ ||
            (this->count_ != nullptr && *this->count_ > 1)) {
            rem();
            this->data_ = std::exchange(other.data_, nullptr);
            this->count_ = std::exchange(other.count_, nullptr);
        }
        return *this;
    }
    void rem() {
        if (count_ == nullptr) {
            return;
        }
        --(*count_);
        if (*count_ == 0) {
            delete data_;
            delete count_;
            return;
        }
    }
    ~shared_ptr() {
        if (count_ != nullptr) {
            if (*count_ != 1) {
                data_ = nullptr;
                --(*count_);
                count_ = nullptr;
            } else {
                delete data_;
                delete count_;
            }
        }
    }
    T *get() const {
        return data_;
    }
    explicit operator bool() const {
        return !(data_ == nullptr);
    }
    T *operator->() const {
        return data_;
    }
    T &operator*() const {
        return *data_;
    }
    void reset(T *data_ = nullptr) {
        if (this->data_ != data_) {
            --(*this->count_);
            this->count_ = new std::size_t(1);
        }
        this->data_ = data_;
    }
    void swap(shared_ptr &other) & {
        using std::swap;
        swap(this->data_, other.data_);
        swap(this->count_, other.count_);
    }
};
template <typename T>
void swap(shared_ptr<T> &lptr, shared_ptr<T> &rptr) {
    lptr.swap(rptr);
}
template <typename T>
bool operator==(const shared_ptr<T> &lptr, const shared_ptr<T> &rptr) {
    return lptr.get() == rptr.get();
}
template <typename T>
bool operator!=(const shared_ptr<T> &lptr, const shared_ptr<T> &rptr) {
    return lptr.get() != rptr.get();
}
}  // namespace lab_05::shared
