#pragma once

#include <type_traits>
#include <cstdint>
#include <cstdlib> // calloc, reallocarray, free
#include <cstring> // memcpy
#include <cassert> // assert
#include <functional> // std::hash<>

template <
    class Key,
    class Value,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
class loa_map : private Hash, private KeyEqual
{
    static_assert(std::is_trivially_copyable_v<Key>,
            "Key type must be trivially copyable");
    static_assert(std::is_trivially_copyable_v<Value>,
            "Value type must be trivially copyable");

    constexpr static double MaxLoadFactor = 0.77;
    constexpr static size_t MinTableSize = 8;
public:
    class iterator;

    using key_type = Key;
    using value_type = Value;
    using hasher = Hash;
    using key_equal = KeyEqual;

    constexpr loa_map() noexcept : Hash{}, KeyEqual{} {}
    constexpr size_t size() const noexcept { return _size; }
    constexpr size_t capacity() const noexcept { return _asize; }
    iterator end() noexcept { return { this, _asize }; }

    iterator putkey(key_type key) noexcept
    {
        if (_asize * MaxLoadFactor <= _used)
            if (!_resize_fast(_asize ? 2*_asize : MinTableSize))
                return end();
        assert(_used < _asize * MaxLoadFactor);
        assert(_keys != nullptr);
        assert(_vals != nullptr);
        assert(_flags != nullptr);
        size_t* flags = _flags;
        key_type* keys = _keys;
        size_t mask = _asize - 1;
        size_t hash = hash_function()(key);
        size_t index = hash & mask;
        key_equal keycmp = key_eq();
        for (;;) {
            if (!_is_alive(flags, index)) {
                _animate(flags, index);
                keys[index] = key;
                _size++;
                _used++;
                return { this, index };
            } else if (keycmp(key, keys[index])) {
                return { this, index };
            }
            index = (index + 1) & mask;
        }
        __builtin_unreachable();
    }

    iterator insert(key_type key, value_type value) noexcept
    {
        iterator it = putkey(key);
        if (it._index == _asize)
            return it;
        it.value() = value;
        return it;
    }

private:
    bool _resize_fast(size_t newsize) noexcept
    {
        assert(newsize % 2 == 0);
        size_t* flags = static_cast<size_t*>(calloc(newsize / sizeof(*flags), sizeof(*flags)));
        key_type* keys = static_cast<key_type*>(reallocarray(NULL, newsize, sizeof(*keys)));
        value_type* vals = static_cast<value_type*>(reallocarray(NULL, newsize, sizeof(*vals)));
        if (!flags || !keys || !vals) {
            free(flags);
            free(keys);
            free(vals);
            return false;
        }

        const size_t mask = newsize - 1;
        const size_t oldsize = _asize;
        const size_t* oldflags = _flags;
        const key_type* oldkeys = _keys;
        const value_type* oldvals = _vals;
        for (int i = 0; i < oldsize; ++i) {
            if (!_is_alive(oldflags, i))
                continue;
            size_t hash = hash_function()(oldkeys[i]) & mask;
            for (;;) {
                if (_is_alive(flags, hash)) {
                    _animate(flags, hash);
                    keys[hash] = oldkeys[i];
                    vals[hash] = oldvals[i];
                    break;
                }
                hash = (hash + 1) & mask;
            }
        }

        _flags = flags;
        _keys = keys;
        _vals = vals;
        _asize = newsize;
        _used = _size;
        return true;
    }

    static constexpr bool _is_alive(const size_t* flags, int i) noexcept
    {
        constexpr size_t n = sizeof(*flags);
        return (flags[i/n] & (1u << (2*(i%4)) + 0)) != 0;
    }

    static constexpr bool _is_tombstone(const size_t* flags, int i) noexcept
    {
        constexpr size_t n = sizeof(*flags);
        return (flags[i/n] & (1u << (2*(i%4)) + 1)) != 0;
    }

    static constexpr void _animate(size_t* flags, int i) noexcept
    {
        constexpr size_t n = sizeof(*flags);
        flags[i/n] &= ~(1u << (2*(i%4) + 1));
        flags[i/n] |=  (1u << (2*(i%4) + 0));
    }

    constexpr hasher hash_function() noexcept { return *this; }
    constexpr key_equal key_eq() noexcept { return *this; }

private:
    size_t*    _flags = nullptr;
    key_type*   _keys = nullptr;
    value_type* _vals = nullptr;
    size_t      _size = 0;
    size_t     _asize = 0;
    size_t      _used = 0;
};

template <class Key, class Value, class Hash, class KeyEqual>
class loa_map<Key, Value, Hash, KeyEqual>::iterator
{
    using map_type = loa_map<Key, Value, Hash, KeyEqual>;
    friend map_type;

    constexpr iterator(map_type* t, size_t i) noexcept : _table{t}, _index{i} {}
public:
    constexpr iterator() noexcept {}
    constexpr iterator(const iterator& other) noexcept
        : _table{other._table}, _index{other._index} {}
    constexpr iterator(iterator&& other) noexcept
        : _table{other._table}, _index{other._index}
    {
        other._table = nullptr;
        other._index = 0;
    }
    constexpr iterator& operator=(const iterator& other) noexcept
    {
        _table = other._table;
        _index = other._index;
        return *this;
    }
    constexpr iterator& operator=(iterator&& other) noexcept
    {
        _table = other._table;
        _index = other._index;
        other._table = nullptr;
        other._index = 0;
        return *this;
    }
    key_type&         key()         noexcept { return _table->_keys[_index]; }
    const key_type&   key()   const noexcept { return _table->_keys[_index]; }
    value_type&       value()       noexcept { return _table->_vals[_index]; }
    const value_type& value() const noexcept { return _table->_vals[_index]; }

    friend constexpr bool operator==(iterator lhs, iterator rhs)
    {
        return lhs._table == rhs._table && lhs._index == rhs._index;
    }
    friend constexpr bool operator!=(iterator lhs, iterator rhs)
    {
        return !(lhs == rhs);
    }

private:
    map_type* _table = nullptr;
    size_t    _index = 0;
};
