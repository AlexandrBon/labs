#include <cassert>
#include <cmath>
#include <iterator>
#include <list>
#include <optional>
#include <type_traits>
#include <utility>

namespace lab_03 {
template <typename T, typename Compare = std::less<T>>
struct list_heap {
private:
    Compare compare_;

    std::list<T> data_{};

    std::optional<T> min_;

    typename std::list<T>::iterator itmin = data_.begin();

    typename std::list<T>::iterator it = data_.begin();

public:
    list_heap() = default;

    explicit list_heap(const Compare &compare) : compare_(compare) {
    }

    list_heap(const list_heap &) = delete;
    list_heap &operator=(const list_heap &) = delete;
    list_heap(list_heap &&) noexcept = default;
    list_heap &operator=(list_heap &&) noexcept = default;
    ~list_heap() = default;

    [[nodiscard]] bool empty() const & {
        return data_.empty();
    }

    [[nodiscard]] const T &top() const & {
        return *min_;
    }

    void push(const T &value) & {
        data_.push_back(value);
        if (!min_ || compare_(value, *min_)) {
            min_.emplace(value);
            itmin = --data_.end();
        }
    }

    void pop() & {
        data_.erase(itmin);

        if (!data_.empty()) {
            min_.emplace(*data_.begin());
            itmin = data_.begin();

            for (it = data_.begin(); it != data_.end(); ++it) {
                if (compare_(*it, *min_)) {
                    min_.emplace(*it);
                    itmin = it;
                }
            }
        }
    }

    void merge(list_heap<T, Compare> &other) & {
        if (!other.empty()) {
            if (!this->min_ || (compare_(*other.min_, *this->min_))) {
                this->min_ = other.min_;
                this->itmin = other.itmin;
            }
        }

        if (!other.empty()) {
            other.min_.reset();
        }

        this->data_.splice(this->data_.begin(), other.data_);
    }
};
