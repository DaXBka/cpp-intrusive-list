#pragma once

#include <algorithm>

namespace project {

class ListHook {
    template<class T>
    friend class List;

public:
    ListHook() = default;

    ListHook(const ListHook &) = delete;

    ListHook(ListHook &&other) noexcept {
        next_ = other.next_;
        prev_ = other.prev_;

        other.next_ = nullptr;
        other.prev_ = nullptr;
    }

    ListHook& operator=(const ListHook &) = delete;

    ListHook& operator=(ListHook &&other) noexcept {
        next_ = other.next_;
        prev_ = other.prev_;

        other.next_ = nullptr;
        other.prev_ = nullptr;

        return *this;
    }

    ~ListHook() {
        Unlink();
    };

    bool IsLinked() const {
        return next_ != nullptr && prev_ != nullptr;
    };

    void Unlink() {
        if (IsLinked()) {
            prev_->next_ = next_;
            next_->prev_ = prev_;
            next_ = nullptr;
            prev_ = nullptr;
        }
    };

private:
    void LinkBefore(ListHook *other) {
        if (other->prev_ != nullptr) {
            other->prev_->next_ = this;
        }

        prev_ = other->prev_;
        other->prev_ = this;
        next_ = other;
    };

    ListHook *next_ = nullptr;
    ListHook *prev_ = nullptr;
};

template<typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator() = delete;

        explicit Iterator(T *elem) : pointer_(elem) {
            assert(elem != nullptr && "Iterator: Pointer must not be null");
        }

        Iterator &operator++() {
            pointer_ = CastToType(pointer_->next_);
            return *this;
        };

        Iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        };

        T &operator*() const {
            return *pointer_;
        };

        T *operator->() const {
            return pointer_;
        };

        bool operator==(const Iterator &rhs) const {
            return pointer_ == rhs.pointer_;
        };

        bool operator!=(const Iterator &rhs) const {
            return pointer_ != rhs.pointer_;
        };

    private:
        T *pointer_ = nullptr;
    };

public:
    List() = default;

    List(const List &) = delete;

    List(List &&other) noexcept {
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;

        if (dummy_.prev_ != nullptr) {
            dummy_.prev_->next_ = &dummy_;
        }

        if (dummy_.next_ != nullptr) {
            dummy_.next_->prev_ = &dummy_;
        }

        other.dummy_ = ListHook{};
    };

    List &operator=(const List &) = delete;

    List &operator=(List &&other) noexcept {
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;

        if (dummy_.prev_ != nullptr) {
            dummy_.prev_->next_ = &dummy_;
        }

        if (dummy_.next_ != nullptr) {
            dummy_.next_->prev_ = &dummy_;
        }

        other.dummy_ = ListHook{};
        return *this;
    };

    ~List() {
        while (dummy_.next_ != nullptr) {
            PopBack();
        }
    };

    bool IsEmpty() const {
        const size_t empty_size = 0u;
        return Size() == empty_size;
    };

    size_t Size() const {
        size_t size = 0u;
        auto cur = dummy_.next_;

        while (cur != nullptr && cur != &dummy_) {
            ++size;
            cur = cur->next_;
        }

        return size;
    };

    void PushBack(T *elem) {
        if (elem == nullptr) {
            return;
        }

        if (dummy_.prev_ == nullptr) {
            dummy_.next_ = elem;
            dummy_.prev_ = elem;
            elem->next_ = &dummy_;
            elem->prev_ = &dummy_;
            return;
        }

        elem->LinkBefore(&dummy_);
    };

    void PushFront(T *elem) {
        if (elem == nullptr) {
            return;
        }

        if (dummy_.next_ == nullptr) {
            dummy_.next_ = elem;
            dummy_.prev_ = elem;
            elem->next_ = &dummy_;
            elem->prev_ = &dummy_;
            return;
        }

        elem->LinkBefore(dummy_.next_);
    };

    T &Front() {
        return *CastToType(dummy_.next_);
    };

    const T &Front() const {
        return *CastToType(dummy_.next_);
    };

    T &Back() {
        return *CastToType(dummy_.prev_);
    };

    const T &Back() const {
        return *CastToType(dummy_.prev_);
    };

    void PopBack() {
        if (dummy_.prev_ != nullptr) {
            dummy_.prev_->Unlink();
        }

        if (dummy_.next_ == &dummy_) {
            dummy_.next_ = nullptr;
            dummy_.prev_ = nullptr;
        }
    };

    void PopFront() {
        if (dummy_.next_ != nullptr) {
            dummy_.next_->Unlink();
        }

        if (dummy_.next_ == &dummy_) {
            dummy_.next_ = nullptr;
            dummy_.prev_ = nullptr;
        }
    };

    Iterator Begin() {
        if (dummy_.next_ == nullptr) {
            return End();
        }

        return Iterator(CastToType(dummy_.next_));
    };

    Iterator End() {
        return Iterator(CastToType(&dummy_));
    };

    Iterator IteratorTo(T *element) {
        if (element != nullptr && element->IsLinked()) {
            return Iterator(element);
        }

        return End();
    };

    static T *CastToType(ListHook* ptr) {
        assert(ptr != nullptr && "Cast: Pointer must not be null");
        return static_cast<T*>(ptr);
    }

private:
    ListHook dummy_;
};

template<typename T>
typename List<T>::Iterator begin(List<T> &list) {
    return list.Begin();
}

template<typename T>
typename List<T>::Iterator end(List<T> &list) {
    return list.End();
}

}  // namespace project
