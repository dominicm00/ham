/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_GRAMMAR_NODE_REGISTRY_H
#define HAM_GRAMMAR_NODE_REGISTRY_H


#include <vector>

#include "code/Node.h"


namespace grammar {


class NodeRegistry {
public:
			struct RegistryHook {
				RegistryHook(NodeRegistry& registry)
					:
					fRegistry(registry)
				{
				}

				template<typename NodeType>
				NodeType* operator()(NodeType* node)
				{
					fRegistry.Register(node);
					return node;
				}

			private:
				NodeRegistry&	fRegistry;
			};

								NodeRegistry()	{}

	inline	void				Register(code::Node* node);

			void				DeleteNodes();

			void				Dump() const;

private:
			typedef std::vector<code::Node*> NodeVector;

private:
			NodeVector			fNodes;
};


void
NodeRegistry::Register(code::Node* node)
{
	try {
		fNodes.push_back(node);
	} catch(...) {
		delete node;
		throw;
	}
}


}	// namespace grammar


#endif	// HAM_GRAMMAR_NODE_REGISTRY_H
