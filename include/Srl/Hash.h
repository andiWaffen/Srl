#ifndef SRL_HASH_H
#define SRL_HASH_H

#include "Common.h"
#include "Heap.h"
#include "Aux.h"

#include <cmath>

namespace Srl { namespace Lib {

    namespace Aux {

        size_t hash_fnc(const uint8_t* bytes, size_t nbytes);

        inline size_t round_pow2(size_t sz)
        {
            return std::pow(2, std::ceil(std::log2(sz)));
        }
    }

    template<class T> struct HashSrl {
        size_t operator() (const T& t) const { return Aux::hash_fnc(t.data(), t.size()); }
    };

    template <class Key, class Val, class HashFnc = HashSrl<Key>>
    class HTable {

    public:
        HTable(size_t buckets = 64, double load_factor_ = 1.0) 
            : load_factor(fmax(load_factor_, 0.1)), cap(Aux::round_pow2(buckets)) { }

        ~HTable() { destroy<Val>(); }

        HTable(const HTable& m) = default;

        HTable(HTable&& m) { *this = std::forward<HTable>(m); }

        HTable& operator= (HTable&& m);

        Val* get (const Key& key);
        /* fst -> exists? snd -> entry */
        std::pair<bool, Val*> insert (const Key& key, const Val& val);
        std::pair<bool, Val*> insert_hash (size_t hash, const Val& val);

        void foreach_entry(const std::function<void(size_t, Val&)>& fnc);

        size_t num_entries() const { return this->elements; }

        void clear();

    private:
        struct Entry {
            Entry (size_t hash_, const Val& val_)
                : hash(hash_), val(val_) { }

            size_t hash;
            Entry* next = nullptr;
            Val    val;
        };

        double load_factor;
        size_t cap;

        size_t limit    = 0; 
        size_t elements = 0;

        Entry** table   = nullptr;
        HashFnc hash_fnc;
        Heap    heap;

        void   redistribute();
        size_t get_bucket(size_t hash);

        Entry** alloc_table();

        template<class T>
        typename std::enable_if<std::is_trivially_destructible<T>::value, void>::type
        destroy() { }

        template<class T>
        typename std::enable_if<!std::is_trivially_destructible<T>::value, void>::type
        destroy();
    };


} }

#endif
