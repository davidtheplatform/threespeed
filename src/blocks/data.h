#pragma once
#include "../types/ScratchTypes.h"
#include "../rendering/Monitor.h"

namespace ts::block {
    template<typename T1, typename T2>
    void data_setvariableto(const T1& value, T2 &variable) {
        variable = T2(value);
    }

    template<typename T1, typename T2>
    void data_changevariableby(const T1& value, T2 &variable) {
        variable = T2((double) variable + (double) value);
    }

    void data_showvariable(VariableMonitor *monitor);
    void data_hidevariable(VariableMonitor *monitor);

    template<typename T1, typename T2>
    void data_addtolist(const T1& value, ts::List<T2>& list) {
        list.append(T2(value));
    }

    template<typename T>
    void data_deleteoflist(ts::Number i, ts::List<T>& list) {
        list.remove(i);
    }

    template<typename T1, typename T2>
    void data_deleteoflist(T1 s, ts::List<T2>& list) {
        if ((std::string) s == "all") {
            list.clear();
        }
        list.remove((int) s);
    }

    template<typename T>
    void data_deletealloflist(ts::List<T>& list) {
        list.clear();
    }

    template<typename T1, typename T2>
    void data_insertatlist(const T1& value, int i, ts::List<T2>& list) {
        list.insert_at(i, value);
    }

    template<typename T1, typename T2>
    void data_replaceitemoflist(const T1& value, int i, ts::List<T2>& list) {
        list.replace(i, T2(value));
    }

    template<typename T>
    T data_itemoflist(ts::Number i, ts::List<T>& list) {
        return list.at(i);
    }

    template<typename T1, typename T2>
    int data_itemnumoflist(const T1& value, ts::List<T2>& list) {
        return list.index_of(T2(value));
    }

    template<typename T>
    int data_lengthoflist(ts::List<T>& list) {
        return list.size();
    }

    template<typename T1, typename T2>
    bool data_listcontainsitem(const T1& value, ts::List<T2>& list) {
        return list.index_of(T2(value)) != 0;
    }

    // TODO
    template<typename T>
    void data_showlist(ts::List<T>&) {}
    template<typename T>
    void data_hidelist(ts::List<T>&) {}
};