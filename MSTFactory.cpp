#include "MSTFactory.h"

std::unique_ptr<IMSTSolver> MSTFactory::createMST(MSTType type, const Graph& graph) {
    if (type == MSTType::KRUSKAL) {
        return std::make_unique<KruskalMST>(graph);
    } else if (type == MSTType::PRIM) {
        return std::make_unique<PrimMST>(graph);
    }
    return nullptr;
}
