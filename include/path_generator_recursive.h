#ifndef _PATH_GENERATOR_RECURSIVE_H
#define _PATH_GENERATOR_RECURSIVE_H

#include "path_generator.h"

template <class Path>
class PathGeneratorRecursive : PathGenerator<Path>
{
	public:
		PathGeneratorRecursive(
				const Fst<Arc>& fst, 
				const Nodes<Arc>& nodes, 
				const PathTerminator<Arc>& pathTerminator, 
				PathGenerator::FinalNodePolicy finalNodePolicy) : 
			PathGenerator<Path>(fst, nodes, pathTerminator, finalNodePolicy)
		{
		}

		virtual void GeneratePaths(int startStateId, float startTime, OverlappingPathGroupList<Path>* pPaths)
		{
			DBG("PathGenerator::GeneratePaths()");
			PathAvgWeight<Arc> init_path(startStateId, startTime);
//			PathMultWeight<Arc> init_path(startStateId, startTime);
			Path<Arc>::SetPrintType(PRINT_ALL);
			//Path<Arc>::SetPrintType(PRINT_NODES_ONLY);
			GeneratePaths_recursive(startStateId, init_path, pPaths);
		}
	protected:

		void GeneratePaths_recursive(typename Arc::StateId stateId, Path<Arc>& path, OverlappingPathGroupList<Path> *pPaths)
		{
//			DBG("GeneratePaths_recursive() path:"<<path);
			ArcIterator< Fst<Arc> > iarc(mFst, stateId);
			// FINAL STATE REACHED
			if (iarc.Done())
			{
				DBG("Final state reached!");
				exit(1);
				// Skip dead-ends (with zero probability).
				if (mFinalNodePolicy == FINAL_NODE_ADD_PATH && mFst.Final(stateId) != Arc::Weight::Zero()) {
					cout << "Adding path: " << path << endl;
					pPaths->Add(path);
				}
			}
			// THERE ARE SOME LINKS FROM THE CURRENT STATE
			else
			{
				const Node<Arc>& n = mNodes[stateId];
				// Loop through n.GetParallelArcs()
				for (typename Node<Arc>::Nextnode2ParallelArcs::const_iterator i = n.GetParallelArcs().begin(); i != n.GetParallelArcs().end(); i++) 
				{
					int nextstate = i->first;
					const ParallelArcs<Arc>& pa = i->second;
//					DBG("pa: "<<pa);
					bool include_arc = false;
					bool path_end = mfPathTerminator(stateId, pa, &include_arc);

					if (path_end && !include_arc) { cout << "Adding path: " << path << endl; pPaths->Add(path); }
					path.push_back(&pa);
					if (path_end && include_arc) { cout << "Adding path: " << path << endl; pPaths->Add(path); }
					// dive into the link only when it is not a "terminal" link
					if (!path_end) {
						GeneratePaths_recursive(nextstate, path, pPaths);
					}
					path.pop_back();
				}
			}
		}

};

#endif
