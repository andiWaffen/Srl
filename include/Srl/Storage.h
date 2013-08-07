#ifndef SRL_STORAGE_H
#define SRL_STORAGE_H

#include "Node.h"
#include "Value.h"
#include "Blocks.h"
#include "String.h"
#include "Hash.h"

namespace Srl {
    class Tree;

    namespace Lib {

    template<class T> struct Link {
        size_t hash;
        T      field;

        Link(size_t hash_, const T& field_)
            : hash(hash_), field(field_) { }
    };

    template<> struct HashSrl<size_t> {
        size_t operator()(size_t s) const
        {
            return s;
        }
    };

    template<> struct HashSrl<const void*> {
        size_t operator()(const void* v) const
        {
            size_t addr = (size_t)v;
            return HashSrl<size_t>()(addr);
        }
    };

    class Storage {
        friend class Srl::Node;
        friend class Srl::Tree;

    public :
        static const Encoding Name_Encoding = Encoding::UTF8;

        Storage() { }
        ~Storage();
        Storage& operator= (Storage&& g);
        Storage(Storage&& g);

        Link<Node>*  store_node  (const Node& node, Tree& tree, const String& name);
        Link<Node>*  create_node (Tree& tree, Type type, const String& name);
        Link<Value>* store_value (const Value& value, const String& name);

        size_t hash_string       (const String& str);
        String conv_string       (const String& str);

        inline std::vector<uint8_t>&  str_conv_buffer();
        inline std::vector<uint8_t>&  type_conv_buffer();
        inline std::list<Node*>&       nodes();

   private :
        Heap<Link<Value>>      value_heap;
        Heap<Link<Node>>       node_heap;
        Heap<uint8_t>          data_heap;
        HTable<String, String> str_table;

        HTable<const void*, size_t>           shared_table_store { 16 };
        HTable<size_t, std::shared_ptr<void>> shared_table_restore { 16 };

        std::list<Node*>      stored_nodes;
        std::vector<uint8_t>  str_buffer;
        std::vector<uint8_t>  type_buffer;

        void clear_nodes();
        template<class T>
        Link<T>* create_link(const T& val, const String& name, Heap<Link<T>>& heap);
    };
} }

#endif
