//
//  Set.h
//  Set
//
//  Created by Antonio Seprano on 10/08/14.
//  Modified by Tobias Niederbrunner 2023
//  Copyright (c) 2014 Antonio Seprano. All rights reserved.
//

#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// concepts


template<typename T>
concept Equal = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};

template<typename T>
concept SetElement = Equal<T>;

//TODO: Ordered Set should only constrain teh element to be std::less orderable, e.g. >, not also ==, fix that by making an inner class, that just requires some sort of comparisons, and can be expressed using == or > !!

// UnorderedSet

template<SetElement _T, class _Allocator = std::allocator<_T>>
class UnorderedSet {
public:
    typedef _T value_type;
    typedef _Allocator allocator_type;

private:
    typedef UnorderedSet<value_type, allocator_type> _SET;

public:
    typedef typename std::vector<value_type, allocator_type>::iterator iterator;
    typedef typename std::vector<value_type, allocator_type>::const_iterator const_iterator;

    /** Empty Unordered Set
     * @brief Creates an Empty Unordered Set
    */
    UnorderedSet(){};

    /** Filled Unordered Set
     * @brief Creates an unordered set with initializer list
     * @param list The initializer list of values
    */
    UnorderedSet(const std::initializer_list<value_type>& list) {
        _set.reserve(list.size());

        for (const auto& value : list) {
            insert(value);
        }
    }

    /** Filled UnorderedSet
     * @brief Creates an unordered set with initializer list of moveable values
     * @param list The initializer list of moveable values
    */
    UnorderedSet(std::initializer_list<value_type>&& l) {
        _set.reserve(l.size());

        for (auto& value : l) {
            insert(std::move(value));
        }
    }

    /** Copy constructor
     * @brief Copies a set, no deep copy is performed, so these are the same Sets, modifying one, modifies the other one!
     * @param list The set to copy
    */
    UnorderedSet(const _SET& s) {
        _set = s._set;
    }

    // Move constructor
    UnorderedSet(_SET&& s) {
        _set = std::move(s._set);
    }

    // Copy assignment
    _SET& operator=(const _SET& s) noexcept {
        _set = s._set;
        return *this;
    }

    // Move assignment
    _SET& operator=(_SET&& s) noexcept {
        _set = std::move(s._set);
        return *this;
    }

    iterator begin() noexcept {
        return _set.begin();
    }

    const_iterator begin() const noexcept {
        return _set.begin();
    }

    iterator end() noexcept {
        return _set.end();
    };

    const_iterator end() const noexcept {
        return _set.end();
    }

    size_t size() const noexcept {
        return _set.size();
    }

    bool empty() const noexcept {
        return _set.empty();
    }

    void clear() noexcept {
        _set.clear();
    }

    virtual iterator find(const value_type& value) noexcept {
        for (auto it = _set.begin(); it != _set.end(); it++) {
            if (*it == value) {
                return it;
            }
        }

        return _set.end();
    }

    const_iterator find(const value_type& value) const noexcept {
        return (const_iterator) find(value);
    }

    virtual size_t count(const value_type& value) const noexcept {
        size_t ret{};

        for (const auto& v : _set) {
            if (v == value) {
                ret++;
            }
        }

        return ret;
    }

    bool operator==(const _SET& s) const noexcept {
        return this->contains(s, false) && s.contains(*this, false);
    }

    bool operator!=(const _SET& s) const noexcept {
        return !(*this == s);
    };

    bool operator<(const _SET& s) const noexcept {
        if (size() != s.size()) {
            return size() < s.size();
        }

        auto it1 = begin(), it2 = s.begin();

        while (it1 != end() && it2 != s.end()) {
            if (*it1 < *it2) {
                return true;
            } else if (*it1 > *it2) {
                return false;
            }

            it1++;
            it2++;
        };

        return false;
    }


    // Insert new value
    // unique = true => values must be unique (no duplicate values allowed)
    virtual _SET& insert(const value_type& value, bool unique = false) noexcept {
        if (!unique || this->find(value) == _set.end()) {
            _set.push_back(value);
        }

        return *this;
    }

    virtual _SET& insert(value_type&& value, bool unique = false) noexcept {
        if (!unique || find(value) == _set.end()) {

            _set.push_back(std::move(value));
        }

        return *this;
    }


    _SET operator+(const value_type& value) const noexcept {
        return _SET{ *this } + _SET{ value };
    }

    _SET operator+(value_type&& value) const noexcept {
        return _SET{ *this } + _SET{ std::move(value) };
    }

    _SET operator+(const _SET& s) const noexcept {
        _SET ret{ *this };

        for (const auto& value : s) {
            ret.insert(value);
        }

        return ret;
    }

    _SET operator+(_SET&& s) const noexcept {
        _SET ret{ *this };

        for (auto& value : s) {
            ret.insert(std::move(value));
        }

        return ret;
    }


    _SET& operator+=(const value_type& value) noexcept {
        insert(value);
        return *this;
    }

    _SET& operator+=(value_type&& value) noexcept {
        insert(std::move(value));
        return *this;
    }

    _SET& operator+=(const _SET& s) noexcept {
        for (const auto& value : s) {
            insert(value);
        }

        return *this;
    }

    _SET& operator+=(_SET&& s) noexcept {
        for (auto& value : s) {
            insert(std::move(value));
        }

        return *this;
    }


    _SET operator-(const value_type& value) const noexcept {
        _SET ret{ *this };
        ret.erase(value);
        return ret;
    }

    _SET operator-(const _SET& s) const noexcept {
        _SET ret{ *this };
        for (const auto& v : s) {
            ret.erase(v);
        }
        return ret;
    }

    _SET& operator-=(const value_type& value) noexcept {
        _set.erase(value);
        return *this;
    }

    _SET& operator-=(const _SET& s) noexcept {
        for (const auto& value : s) {
            _set.erase(value);
        }
        return *this;
    }


    UnorderedSet<_SET> operator*(const _SET& other) noexcept {
        UnorderedSet<_SET> ret;

        if (!this->empty() && !other.empty()) {
            for (const auto& v1 : _set) {
                for (const auto& v2 : other) {
                    ret.insert(_SET{ v1, v2 });
                }
            }
        }

        return ret;
    }


    _SET intersectionWith(const _SET& s) noexcept {
        _SET s1, s2;

        if (s.size() <= size()) {
            s1 = s;
            s2 = *this;
        } else {
            s1 = *this;
            s2 = s;
        }

        for (const auto& v : s1) {
            if (!s2.contains(v))
                s1.erase(v);
        }

        return s1;
    }

    bool contains(const value_type& value) const noexcept {
        return contains(_SET{ value });
    }

    virtual bool contains(const _SET& s, bool strict = false) const noexcept {
        if (strict) {
            return this->contains(s, false) && s.contains(*this, false);
        }

        for (const auto& value : s) {
            bool found{ false };

            for (const auto& v : _set) {
                if (value == v) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }


        return true;
    }

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (auto it = first; it != last; it++) {
            _set.insert(*it);
        }
    }

    iterator erase(const_iterator position) {
        return _set.erase(position);
    }

    iterator erase(const_iterator first, const_iterator last) {
        return _set.erase(first, last);
    }

    size_t erase(const value_type& value, bool all = false) {
        size_t ret{};

        if (!all) {
            auto it = find(value);

            if (it != end()) {
                erase(it);
                ret++;
            }

            return ret;
        }

        for (auto it = _set.begin(); it != _set.end(); it++) {
            if (*it == value) {
                _set.erase(it);
                ++ret;
            }
        }

        return ret;
    }

    virtual _SET& unique() noexcept {
        for (auto i = begin(); i != end(); i++) {
            for (auto j = i + 1; j != end(); j++) {
                if (i == j) {
                    _set.erase(j);
                }
            }
        }

        return *this;
    }

    UnorderedSet<_SET> combinations(int n) const {
        UnorderedSet<_SET> ret;

        if (n > _set.size()) {
            throw std::out_of_range("Value out of range for set combinations");
        }

        std::vector<int> counters(n);
        for (int i = 0; i < n; i++) {
            counters[i] = i;
        }

        do {
            _SET s;

            for (auto index : counters) {
                s += _set[index];
            }

            ret += s;
        } while (this->_increase_counters(counters));

        return ret;
    }

public:
    std::vector<value_type, allocator_type> _set;

private:
    bool _increase_counters(std::vector<int>& c) const noexcept {
        long int limit = _set.size() - 1;
        long int i = c.size() - 1;
        bool found{ false };

        do {
            if (++c[i] <= (limit - (c.size() - i - 1))) {
                found = true;
            } else {
                if (--i < 0) {
                    return false;
                }
            }
        } while (!found);

        i++;

        while (i < c.size()) {
            c[i] = c[i - 1] + 1;
            i++;
        }

        return true;
    }
};


template<typename T>
concept CompareFunction = std::is_function<T>::value && std::is_convertible<T, std::function<bool(T, T)>>::value;


template<SetElement _T, CompareFunction _Compare = std::less<_T>, class _Allocator = std::allocator<_T>>
class OrderedSet : public UnorderedSet<_T, _Allocator> {
public:
    typedef _T value_type;
    typedef _Allocator allocator_type;
    typedef _Compare value_compare;

private:
    typedef OrderedSet<value_type, value_compare, allocator_type> _SET;

public:
    typedef typename std::vector<value_type, allocator_type>::iterator iterator;
    typedef typename std::vector<value_type, allocator_type>::const_iterator const_iterator;

    /** Empty Set
     * @brief Creates an Empty Set
    */
    OrderedSet(){};

    /** Filled Set
     * @brief Creates an set with initializer list
     * @param list The initializer list of values
    */
    OrderedSet(const std::initializer_list<value_type>& list)
        : UnorderedSet<_T, _Allocator>{ std::forward<value_type>(list) } { }

    /** Filled Set
     * @brief Creates an set with initializer list of moveable values
     * @param list The initializer list of moveable values
    */
    OrderedSet(std::initializer_list<value_type>&& l) : UnorderedSet<_T, _Allocator>{ std::forward<value_type>(l) } { }

    /** Copy constructor
     * @brief Copies a set, no deep copy is performed, so these are the same Sets, modifying one, modifies the other one!
     * @param list The set to copy
    */
    OrderedSet(const _SET& s) {
        this->_set = s._set;
    }

    // Move constructor
    OrderedSet(_SET&& s) {
        this->_set = std::move(s._set);
    }


    iterator find(const value_type& value) noexcept override {
        for (auto it = this->_set.begin(); it != this->_set.end(); it++) {
            if (!value_compare()(*it, value) && !value_compare()(value, *it)) {
                return it;
            }
        }

        return this->_set.end();
    }

    size_t count(const value_type& value) const noexcept override {
        size_t ret{};

        for (const auto& v : this->_set) {
            if (!value_compare()(v, value) && !value_compare()(value, v)) {
                ret++;
            }
        }

        return ret;
    }

    // Insert new value
    // unique = true => values must be unique (no duplicate values allowed)
    _SET& insert(const value_type& value, bool unique = false) noexcept override {
        if (!unique || this->find(value) == this->_set.end()) {
            auto it = this->begin();

            while (it != this->end() && value_compare()(*it, value)) {
                it++;
            }

            this->_set.insert(it, value);
        }

        return *this;
    }

    _SET& insert(value_type&& value, bool unique = false) noexcept override {
        if (!unique || find(value) == this->_set.end()) {
            auto it = this->begin();

            while (it != this->end() && value_compare()(*it, value)) {
                it++;
            }

            this->_set.insert(it, std::move(value));
        }

        return *this;
    }

    bool contains(const _SET& s, bool strict = false) const noexcept override {
        if (strict) {
            return this->contains(s, false) && s.contains(*this, false);
        }

        for (const auto& value : s) {
            bool found{ false };

            for (const auto& v : this->_set) {
                if (!value_compare()(value, v) && !value_compare()(v, value)) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }


        return true;
    }

    _SET& unique() noexcept override {
        for (auto i = this->begin(); i != this->end(); i++) {
            auto j = i + 1;

            while (j != this->end()) {
                if (!value_compare()(*i, *j) && !value_compare()(*j, *j)) {
                    this->_set.erase(j);
                } else {
                    j++;
                }
            }
        }

        return *this;
    }
};
