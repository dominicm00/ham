/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/NodeRegistry.h"

#include <stdio.h>

#include <algorithm>


namespace grammar {


void
NodeRegistry::DeleteNodes()
{
	NodeVector::iterator it = fNodes.begin();
	while (it != fNodes.end()) {
		code::Node* node = *it;

		// visit all descendents of the node and remove them from the vector
		struct RemoveVisitor : code::NodeVisitor {
			RemoveVisitor(code::Node* node, NodeVector& nodes)
				:
				fRoot(node),
				fNodes(nodes)
			{
			}

			virtual bool VisitNode(code::Node* node)
			{
				if (node != fRoot) {
					NodeVector::iterator it = std::find(fNodes.begin(),
						fNodes.end(), node);
					if (it != fNodes.end())
						fNodes.erase(it);
				}

				return false;
			}

		private:
			code::Node*		fRoot;
			NodeVector&		fNodes;
		} visitor(node, fNodes);

		node->Visit(visitor);

		it = std::find(fNodes.begin(), fNodes.end(), node);
		++it;
	}

printf("NodeRegistry::DeleteNodes(): deleting %zu nodes\n", fNodes.size());
	for (NodeVector::iterator it = fNodes.begin(); it != fNodes.end(); ++it) {
printf("  %p\n", *it);
		delete *it;
	}
}


void
NodeRegistry::Dump() const
{
	printf("NodeRegistry: %zd nodes:\n", fNodes.size());

	for (NodeVector::const_iterator it = fNodes.begin(); it != fNodes.end();
			++it) {
		printf("  %p\n", *it);
	}
}


}	// namespace grammar
