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
#include <format>
#include <functional>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>


// concepts

namespace internals {

    //from https://www.reddit.com/r/cpp/comments/fblqwd/is_printable_check_if_a_value_is_printable_at/
    template<typename T>
    concept Printable = requires(T t) {
        { std::cout << t } -> std::same_as<std::ostream&>;
    };

    // all selfmade templates

    template<typename T>
    concept HasOperatorCall = requires() { T(); };

    template<typename T, typename _Compare>
    concept CompareFunction =
            HasOperatorCall<_Compare> && (std::is_function<_Compare()>::value || std::is_invocable<_Compare()>::value)
            && std::is_convertible<_Compare, std::function<bool(T, T)>>::value;

    template<typename _T>
    concept IsLiteralTrue = std::integral_constant<_T, true>::value;


    template<typename T, typename _Compare>
    concept SymmetricOp = CompareFunction<T, _Compare> && requires(T a, T b) {
        { _Compare()(a, b) == _Compare()(b, a) } -> IsLiteralTrue<>;
    };

    // TODO: this doesn't seem to work correctly!
    template<typename T, typename _Compare>
    concept CompareFunctionInstance = CompareFunction<T, _Compare> && requires(T a, T b) {
        _Compare()(a, b);
        { _Compare()(a, b) } -> std::convertible_to<bool>;
    };


} // namespace internals

// UnorderedSet

template<typename _T, typename _Compare = std::equal_to<_T>, class _Allocator = std::allocator<_T>>
    requires internals::CompareFunctionInstance<_T, _Compare> && internals::SymmetricOp<_T, _Compare>
class UnorderedSet {
public:
    typedef _T value_type;
    typedef _Allocator allocator_type;
    typedef _Compare _equal_compare;

private:
    typedef UnorderedSet<value_type, _Compare, allocator_type> _SET;

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
     * @brief Copies a set, a deep copy is performed, depending on the input type, the behaviour depends on that, so don't expect anything, test if the expected behaviour is given!  
     * @param list The set to copy
    */
    UnorderedSet(const _SET& s) {
        _set = s._set;
    }

    // Move constructor
    UnorderedSet(_SET&& s) {
        _set = std::move(s._set);
    }

    static _SET deepCopy(const _SET& s) {
        _SET result{};
        for (const auto& value : s) {
            result.insert(value);
        }
        return result;
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
        return _set.cbegin();
    }

    iterator end() noexcept {
        return _set.end();
    };

    const_iterator end() const noexcept {
        return _set.cend();
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

    iterator find(const value_type& value) noexcept {
        for (auto it = _set.begin(); it != _set.end(); it++) {
            if (_equal_compare()(*it, value)) {
                return it;
            }
        }

        return _set.end();
    }

    const_iterator find(const value_type& value) const noexcept {
        return static_cast<const_iterator>(find(value));
    }

    size_t count(const value_type& value) const noexcept {
        size_t ret{};

        for (const auto& v : _set) {
            if (_equal_compare()(v, value)) {
                ret++;
            }
        }

        return ret;
    }

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
    _SET& insert(const value_type& value, bool unique = true) noexcept {
        if (!unique || this->find(value) == _set.end()) {
            _set.push_back(value);
        }

        return *this;
    }

    _SET& insert(value_type&& value, bool unique = true) noexcept {
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
        this->erase(value);
        return *this;
    }

    _SET& operator-=(const _SET& s) noexcept {
        for (const auto& value : s) {
            this->erase(value);
        }
        return *this;
    }

    template<typename OtherCompare>
    bool operator==(const UnorderedSet<_T, OtherCompare>& other) const noexcept {
        if (this->size() != other.size()) {
            return false;
        }

        return this->contains(other, true);
    }

    template<typename OtherCompare, typename OtherAlloc>
    bool operator!=(const UnorderedSet<_T, OtherCompare, OtherAlloc>& other) const noexcept {
        return !(*this == other);
    }

    template<typename OtherCompare, typename OtherAlloc>
    UnorderedSet<_SET, OtherCompare, OtherAlloc> operator*(const _SET& other) noexcept {
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


    _SET intersectionWith(_SET& s) noexcept {
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

    _SET intersectionWith(const _SET& s) const noexcept  {
        _SET s1{}, s2{};

        if (s.size() <= size()) {
            s1 += s;
            s2 += *this;
        } else {
            s1 += *this;
            s2 += s;
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

    bool contains(const UnorderedSet<_T>& s, bool strict = false) const noexcept {
        if (strict) {
            return this->contains(s, false) && s.contains(*this, false);
        }

        for (const auto& value : s) {
            bool found{ false };

            for (const auto& v : _set) {
                if (_equal_compare()(value, v)) {
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


    bool subseteq(const UnorderedSet<_T>& s) const noexcept {

        if (*this == s) {
            return true;
        }

        for (const auto& value : _set) {
            bool found{ false };

            for (const auto& v : s) {
                if (_equal_compare()(value, v)) {
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


    bool subset(const UnorderedSet<_T>& s) const noexcept {
        if (*this == s) {
            return false;
        }

        return subseteq(s);
    }

    bool superseteq(const UnorderedSet<_T>& s) const noexcept {

        if (*this == s) {
            return true;
        }

        for (const auto& value : s) {
            bool found{ false };

            for (const auto& v : _set) {
                if (_equal_compare()(value, v)) {
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

    bool superset(const UnorderedSet<_T>& s) const noexcept {
        if (*this == s) {
            return false;
        }

        return superset(s);
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
            if (_equal_compare()(*it, value)) {
                _set.erase(it);
                ++ret;
            }
        }

        return ret;
    }

    _SET& unique() noexcept {
        for (auto i = begin(); i != end(); i++) {
            for (auto j = i + 1; j != end(); j++) {
                if (i == j) {
                    _set.erase(j);
                }
            }
        }

        return *this;
    }

    template<typename Compare2, typename Alloc2>
    UnorderedSet<_SET, Compare2, Alloc2> combinations(int n) const {
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


namespace internals {

    template<typename _Tp, typename _Compare>
        requires internals::CompareFunction<_Tp, _Compare>
    struct ComparatorEqual : public std::function<bool(_Tp, _Tp)> {

        constexpr bool operator()(const _Tp& x, const _Tp& y) const {
            return !_Compare()(x, y) && !_Compare()(y, x);
        }
    };
} // namespace internals

template<typename _T, typename _Compare = std::less<_T>, class _Allocator = std::allocator<_T>>
    requires internals::CompareFunctionInstance<_T, _Compare>
class OrderedSet : public UnorderedSet<_T, internals::ComparatorEqual<_T, _Compare>, _Allocator> {
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
        : UnorderedSet<_T, internals::ComparatorEqual<_T, _Compare>, _Allocator>{} {
        this->_set.reserve(list.size());

        for (const auto& value : list) {
            insert(value);
        }
    }

    /** Filled Set
     * @brief Creates an set with initializer list of moveable values
     * @param list The initializer list of moveable values
    */
    OrderedSet(std::initializer_list<value_type>&& l)
        : UnorderedSet<_T, internals::ComparatorEqual<_T, _Compare>, _Allocator>{} {
        this->_set.reserve(l.size());

        for (auto& value : l) {
            insert(std::move(value));
        }
    }

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


    iterator find(const value_type& value) noexcept {
        for (auto it = this->_set.begin(); it != this->_set.end(); it++) {
            if (!value_compare()(*it, value) && !value_compare()(value, *it)) {
                return it;
            }
        }

        return this->_set.end();
    }

    size_t count(const value_type& value) const noexcept {
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
    _SET& insert(const value_type& value, bool unique = true) noexcept {
        if (!unique || this->find(value) == this->_set.end()) {
            auto it = this->begin();

            while (it != this->end() && value_compare()(*it, value)) {
                it++;
            }

            this->_set.insert(it, value);
        }

        return *this;
    }

    _SET& insert(value_type&& value, bool unique = true) noexcept {
        if (!unique || find(value) == this->_set.end()) {
            auto it = this->begin();

            while (it != this->end() && value_compare()(*it, value)) {
                it++;
            }

            this->_set.insert(it, std::move(value));
        }

        return *this;
    }

    template<typename _OtherComp, typename OtherAlloc>
    bool contains(const OrderedSet<_T, _OtherComp, OtherAlloc>& s, bool strict = false) const noexcept {
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


    _SET& unique() noexcept {
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

namespace internals {

    struct PrintOptions {
        bool compact;
        bool trailingSeparator;
        std::string separator;

        constexpr PrintOptions(bool compact = true, bool trailingSeparator = false, std::string separator = ",")
            : compact{ compact },
              trailingSeparator{ trailingSeparator },
              separator{ separator } {
            //
        }
    };


    template<internals::Printable T, typename C, typename Allocator>
    std::string set_to_string(const UnorderedSet<T, C, Allocator>& set, PrintOptions options = PrintOptions{}) {

        std::stringstream ss;

        const std::string space = options.compact ? "" : "\t";
        const std::string elementSpacer = options.compact ? " " : "\n";

        bool firstItem{ true };

        if (set.size() == 0) {
            ss << "{}";
            return ss.str();
        }

        ss << "{" << elementSpacer;

        for (const auto& value : set) {
            if (!firstItem) {
                ss << options.separator << elementSpacer;
            } else {
                firstItem = false;
            }

            ss << space << value;
        }
        if (options.trailingSeparator) {
            ss << options.separator;
        }
        ss << elementSpacer << "}";


        return ss.str();
    }

} // namespace internals

template<internals::Printable T, typename C, typename Allocator>
std::ostream& operator<<(std::ostream& os, const UnorderedSet<T, C, Allocator>& set) {
    os << internals::set_to_string(set);
    return os;
}


template<internals::Printable T, typename C, typename Allocator>
std::ostream& operator<<(std::ostream& os, const OrderedSet<T, C, Allocator>& set) {
    os << internals::set_to_string(set);
    return os;
}


/** @brief format for std::deque of formattable type
 *  format options for this are
 *  {:<flags>:<separator>}
 * everything is optional, standard is {:cT:,}
 * the flags section defines these arguments:
 * cC: compact, print it compact, lower case is true, uppercase is false, the last specifier is taken
 * tT: trailingSeparator, print a trailing separator, lower case is true, uppercase is false, the last specifier is taken
 * separator: a string that is taken as separator
*/


template<typename T, typename C, typename Allocator, typename CharT>
struct std::formatter<UnorderedSet<T, C, Allocator>, CharT> : std::formatter<std::string, CharT> {

    internals::PrintOptions options{};


    constexpr auto parse(std::format_parse_context& ctx) {
        auto pos = ctx.begin();
        std::array<std::string, 2> specifier{};
        std::size_t array_idx = 0;
        while (pos != ctx.end() && *pos != '}') {
            if (*pos == ':') {
                ++array_idx;
                if (array_idx >= 2) {
                    throw std::format_error("Format specifier can only have two specifiers!");
                }
                ++pos;
                continue;
            }
            specifier.at(array_idx) += *pos;
            ++pos;
        }

        for (const auto& c : specifier.at(0)) {
            switch (c) {
                case 'c':
                    options.compact = true;
                    break;
                case 'C':
                    options.compact = false;
                    break;
                case 't':
                    options.trailingSeparator = true;
                    break;
                case 'T':
                    options.trailingSeparator = false;
                    break;
                default:
                    throw std::format_error("Unknown Format specifier in first section: allowed are 'cCtT'");
            }
        }

        if (!specifier.at(1).empty()) {
            options.separator = specifier.at(1);
        }


        if (*pos != '}') {
            throw std::format_error("Format specifier doesn't end with '}'");
        }
        return pos;
    }

    auto format(const UnorderedSet<T, C, Allocator>& value, std::format_context& ctx) const {

        return std::format_to(ctx.out(), "{}", internals::set_to_string(value, options));
    }
};

template<typename T, typename C, typename Allocator, typename CharT>
struct std::formatter<OrderedSet<T, C, Allocator>, CharT>
    : std::formatter<UnorderedSet<T, internals::ComparatorEqual<T, C>, Allocator>, CharT> {

    //
};
