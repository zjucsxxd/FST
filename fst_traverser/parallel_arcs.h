#ifndef _PARALLEL_ARCS_H
#define _PARALLEL_ARCS_H

#include <iostream>
#include <list>
#include <fst/fst.h>
#include "container_interface.h"
#include "print_type.h"

template <class Arc>
struct ParallelArcs_Base {
	typedef ContainerInterface< std::list<const Arc*> > type;
};

template <class Arc>
class ParallelArcs : public ParallelArcs_Base<Arc>::type {
	protected:
		typedef typename ParallelArcs_Base<Arc>::type Base;
	public:
		ParallelArcs() : mWeight(Arc::Weight::Zero()) {}

		void Add(const Arc* arc) {
			assert(this->empty() || (*this->begin())->nextstate == arc->nextstate);
			this->mContainer.push_back(arc);
			mWeight = Plus(mWeight, arc->weight);
		}

		inline typename Arc::Weight GetWeight() const { return mWeight; }
		inline int GetNextState() const { return this->empty() ? -1 : (*this->begin())->nextstate; }

		//--------------------------------------------------
		// PRINTING
		//--------------------------------------------------
		std::ostream& PrintNodesOnly(std::ostream& oss) const {
			if (this->empty()) {
				return oss << "[EMPTY]";
			} else {
				return oss << GetNextState();
			}
		}

		std::ostream& PrintAllInfo(std::ostream& oss) const {
			if (this->empty()) {
				return oss << "[EMPTY]";
			} else {
				oss << "[";
				string separator = "";
				for (typename Base::const_iterator i=this->begin(); i!=this->end(); i++) {
					oss << separator;
					const Arc& a = **i;
					if (mspSyms) {oss << mspSyms->Find(a.ilabel);} else {oss << a.ilabel;}
					oss << ":";
					if (mspSyms) {oss << mspSyms->Find(a.olabel);} else {oss << a.olabel;}
					oss << "/" << a.weight;
					separator = " ";
				}
				oss << "] -> " << (*this->begin())->nextstate;
				return oss;
			}
		}

		friend std::ostream& operator<<(std::ostream& oss, const ParallelArcs<Arc>& pa) {
			if (msPrintType == PRINT_ALL) {
				return pa.PrintAllInfo(oss);
			} else if (msPrintType == PRINT_NODES_ONLY) {
				return pa.PrintNodesOnly(oss);
			} else {
				throw std::runtime_error("ERROR: Unknown print type of Path!");
			}
		}
		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		static void SetSymbols(const fst::SymbolTable* syms) { mspSyms = syms; }
		static void SetPrintType(PrintType pt) { msPrintType = pt; }

	protected:
		typename Arc::Weight mWeight;
		static const fst::SymbolTable* mspSyms;
		static PrintType msPrintType;
};

template <class Arc> const fst::SymbolTable* ParallelArcs<Arc>::mspSyms = NULL;
template <class Arc> PrintType ParallelArcs<Arc>::msPrintType = PRINT_ALL;

#endif