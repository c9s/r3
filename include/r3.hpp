/*
 * r3.hpp
 * Copyright (C) 2014 whitglint <whitglint.tw@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#ifndef R3_HPP
#define R3_HPP

#include <cstring>
#include <r3.h>

namespace r3 {
    template <typename T>
    class Base {
    public:
        Base(T* p)
            : p_(p) {
        }

        void* data() const {
            return p_->data;
        }

        T* get() const {
            return p_;
        }

        bool is_null() const {
            return p_ == NULL;
        }

        operator void*() const {
            return p_;
        }

    private:
        T* p_;
    };
    typedef Base<node> Node;
    typedef Base<route> Route;

    class MatchEntry : public Base<match_entry> {
    public:
        explicit MatchEntry(const char* path)
            : Base(match_entry_create(path)) {
        }

        MatchEntry(const char* path, int path_len)
            : Base(match_entry_createl(path, path_len)) {
        }

        ~MatchEntry() {
            if (get()) {
                match_entry_free(get());
            }
        }

        int request_method() const {
            return get()->request_method;
        }

        void set_request_method(int request_method) {
            get()->request_method = request_method;
        }

    private:
        MatchEntry(const MatchEntry&);
        MatchEntry& operator =(const MatchEntry&);
    };

    class Tree : public Base<node> {
    public:
        explicit Tree(int cap)
            : Base(r3_tree_create(cap)) {
        }

        ~Tree() {
            if (get()) {
                r3_tree_free(get());
            }
        }

        int compile(char** errstr = NULL) {
            return r3_tree_compile(get(), errstr);
        }

        void dump(int level) const {
            r3_tree_dump(get(), level);
        }

        Node insert_path(const char* path, void* data, char** errstr = NULL) {
            return r3_tree_insert_pathl_ex(get(), path, std::strlen(path), NULL,
                data, errstr);
        }

        Node insert_pathl(const char* path, int path_len, void* data,
            char** errstr = NULL) {
            return r3_tree_insert_pathl_ex(get(), path, path_len, NULL, data,
                errstr);
        }

        Route insert_route(int method, const char* path, void* data,
            char** errstr = NULL) {
            return r3_tree_insert_routel_ex(get(), method, path,
                std::strlen(path), data, errstr);
        }

        Route insert_routel(int method, const char* path, int path_len,
            void* data, char** errstr = NULL) {
            return r3_tree_insert_routel_ex(get(), method, path, path_len, data,
                errstr);
        }

        Node match(const char* path, MatchEntry* entry = NULL) const {
            return r3_tree_match(get(), path,
                entry != NULL ? entry->get() : NULL);
        }

        Node matchl(const char* path, int path_len, MatchEntry* entry = NULL)
            const {
            return r3_tree_matchl(get(), path, path_len,
                entry != NULL ? entry->get() : NULL);
        }

        Node match_entry(MatchEntry& entry) const {
            return r3_tree_match_entry(get(), entry.get());
        }

        Route match_route(MatchEntry& entry) const {
            return r3_tree_match_route(get(), entry.get());
        }

    private:
        Tree(const Tree&);
        Tree& operator =(const Tree&);
    };
}

#endif // R3_HPP
