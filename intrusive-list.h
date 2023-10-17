#pragma once

#include <algorithm>

class ListHook {
public:
    ListHook(){};

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

    ~ListHook() {
        Unlink();
    };

    ListHook(const ListHook&) = delete;

private:
    template <class T>
    friend class List;

    void LinkBefore(ListHook* other) {
        other->prev_->next_ = this;
        prev_ = other->prev_;
        other->prev_ = this;
        next_ = other;
    };

private:
    ListHook* next_ = nullptr;
    ListHook* prev_ = nullptr;
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        explicit Iterator(T* elem) : pointer_(elem) {
        }

        Iterator& operator++() {
            pointer_ = static_cast<T*>(pointer_->next_);
            return *this;
        };

        Iterator operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        };

        T& operator*() const {
            return *pointer_;
        };

        T* operator->() const {
            return pointer_;
        };

        bool operator==(const Iterator& rhs) const {
            return pointer_ == rhs.pointer_;
        };
        bool operator!=(const Iterator& rhs) const {
            return pointer_ != rhs.pointer_;
        };

    private:
        T* pointer_ = nullptr;
    };

    List()= default;
    List(const List&) = delete;
    List(List&& other) noexcept {
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;

        dummy_.prev_->next_ = &dummy_;
        dummy_.next_->prev_ = &dummy_;

        other.dummy_ = ListHook();
    };

    ~List() {
        while (dummy_.next_ != nullptr) {
            PopBack();
        }
    };

    List& operator=(const List&) = delete;

    List& operator=(List&& other) noexcept {
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;

        dummy_.prev_->next_ = &dummy_;
        dummy_.next_->prev_ = &dummy_;

        other.dummy_ = ListHook();

        return *this;
    };

    bool IsEmpty() const {
        return Size() == 0;
    };

    size_t Size() const {
        size_t size = 0;
        auto cur = dummy_.next_;

        while (cur != nullptr && cur != &dummy_) {
            ++size;
            cur = cur->next_;
        }

        return size;
    };

    void PushBack(T* elem) {
        if (dummy_.prev_ == nullptr) {
            dummy_.next_ = elem;
            dummy_.prev_ = elem;
            elem->next_ = &dummy_;
            elem->prev_ = &dummy_;
            return;
        }

        elem->LinkBefore(&dummy_);
    };

    void PushFront(T* elem) {
        if (dummy_.next_ == nullptr) {
            dummy_.next_ = elem;
            dummy_.prev_ = elem;
            elem->next_ = &dummy_;
            elem->prev_ = &dummy_;
            return;
        }

        elem->LinkBefore(dummy_.next_);
    };

    T& Front() {
        return *static_cast<T*>(dummy_.next_);
    };
    const T& Front() const {
        return *static_cast<T*>(dummy_.next_);
    };

    T& Back() {
        return *static_cast<T*>(dummy_.prev_);
    };
    const T& Back() const {
        return *static_cast<T*>(dummy_.prev_);
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

        return Iterator(static_cast<T*>(dummy_.next_));
    };

    Iterator End() {
        return Iterator(static_cast<T*>(&dummy_));
    };

    Iterator IteratorTo(T* element) {
        if (element->IsLinked()) {
            return Iterator(element);
        }

        return End();
    };

private:
    ListHook dummy_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {
    return list.End();
}
