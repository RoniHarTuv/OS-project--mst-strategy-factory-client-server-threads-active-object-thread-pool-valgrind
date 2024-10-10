#ifndef MST_FACTORY_H
#define MST_FACTORY_H

#include <memory>
#include "IMSTSolver.h"
#include "KruskalMST.h"
#include "PrimMST.h"

enum class MSTType {
    KRUSKAL,
    PRIM
};

class MSTFactory {
public:
    static std::unique_ptr<IMSTSolver> createMST(MSTType type, const Graph& graph);
};

#endif // MST_FACTORY_H
