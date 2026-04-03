#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Alphabet.hpp"
#include "Config.hpp"
#include "util/util.hpp"

using Symbol = std::string;
using SymbolSet = std::set<Symbol>;

class Graph {
  public:
    using Verts = std::vector<Symbol>;
    using Adj = std::vector<std::vector<SymbolSet>>;

    Graph(Config c, Alphabet a) : cfg_(std::move(c)), alpha_(std::move(a)) {
        labels = genLabels();
    }

    void set(const SymbolSet& words) {
        V = genVerts(words);
        A = genAdj(V);
    }

    const std::vector<Symbol>& getV() const {
        return V;
    }

    const Adj& getA() const {
        return A;
    }

    void minimize() {
        while (true) {
            auto newGraph = minimizeOnce();
            if (newGraph.getV().size() == V.size())
                return;
            *this = std::move(newGraph);
        }
    }

  private:
    SymbolSet genLabels() const {
        SymbolSet result;
        const size_t limit = util::calcPower(alpha_.size(), cfg_.T);
        for (size_t i = 0; i < limit; ++i)
            result.insert(alpha_.toSymbol(i, cfg_.T));
        return result;
    }

    Verts genVerts(const SymbolSet& words) const {
        Verts verts{""};
        for (const auto& w : words)
            for (size_t len = cfg_.T; len <= w.size(); len += cfg_.T)
                verts.push_back(w.substr(0, len));
        util::normalize(verts);
        return verts;
    }

    Adj genAdj(const Verts& verts) const {
        std::unordered_map<Symbol, size_t> id = genIdMap(verts);
        Adj adj(verts.size(), std::vector<SymbolSet>(verts.size()));
        for (size_t i = 0; i < verts.size(); ++i)
            if (verts[i].size() < cfg_.L)
                for (const auto& l : labels)
                    adj[i][next(verts[i], l, id)].insert(l);
        return adj;
    }

    std::unordered_map<Symbol, size_t> genIdMap(const Verts& verts) const {
        std::unordered_map<Symbol, size_t> id;
        for (size_t i = 0; i < verts.size(); ++i)
            id[verts[i]] = i;
        return id;
    }

    size_t next(const Symbol& v, const Symbol& l, const std::unordered_map<Symbol, size_t>& id) const {
        Symbol nv = v + l;
        while (true) {
            if (auto it = id.find(nv); it != id.end())
                return it->second;
            nv.erase(0, cfg_.T);
        }
    }

    Graph minimizeOnce() {
        const size_t n = A.size();

        std::unordered_map<std::string, size_t> sig2gid;
        std::vector<std::vector<size_t>> groups;
        std::vector<size_t> gid(n);

        for (size_t i = 0; i < n; ++i) {
            auto sig = signature(A[i]);

            auto [it, inserted] = sig2gid.emplace(sig, groups.size());
            if (inserted)
                groups.emplace_back();

            groups[it->second].push_back(i);
            gid[i] = it->second;
        }

        std::vector<std::string> newV;
        newV.reserve(groups.size());

        for (auto& g : groups) {
            std::string name;
            for (size_t i = 0; i < g.size(); ++i) {
                if (i)
                    name += '+';
                name += V[g[i]];
            }
            newV.push_back(std::move(name));
        }

        Adj newA(groups.size(), std::vector<SymbolSet>(groups.size()));

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                newA[gid[i]][gid[j]].insert(A[i][j].begin(), A[i][j].end());

        return Graph(cfg_, alpha_, newV, newA, labels);
    }

    static std::string signature(const std::vector<SymbolSet>& row) {
        std::vector<std::string> s;
        for (const auto& cell : row)
            s.push_back(util::join(cell, ","));
        return util::join(s, "|");
    }

    Graph(Config c, Alphabet a, Verts v, Adj adj, SymbolSet l)
        : cfg_(std::move(c)), alpha_(std::move(a)), V(std::move(v)), A(std::move(adj)), labels(std::move(l)) {
    }

  private:
    Config cfg_;
    Alphabet alpha_;

    Verts V;
    Adj A;
    std::set<Symbol> labels;
};
