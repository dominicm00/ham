/*
 * Copyright 2010, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */


#include "grammar/Spirit.h"
	// precompiled -- must be first

#include "grammar/Grammar.h"

#include "code/Block.h"
#include "code/RuleDefinition.h"
#include "grammar/ActorFactory.h"
#include "grammar/Iterator.h"
#include "grammar/Skipper.h"


namespace grammar {


namespace phoenix = boost::phoenix;


template<>
void
Grammar<IteratorType, Skipper<IteratorType> >::_InitRuleDefinition()
{
	using qi::_val;
	using qi::_1;

	ActorFactory<Factory> factory(fFactory);

	fRuleDefinition
		= ("rule" >> fIdentifier)
				[ _val = factory.Create<code::RuleDefinition>(_1)]
			>> -(fIdentifier
					[ bind(&code::RuleDefinition::AddParameterName, _val,
						_1) ]
				>> *(':'
					>> fIdentifier
							[ bind(&code::RuleDefinition::AddParameterName,
								_val, _1) ]
					)
				)
			>> '{'
			>> fBlock
					[ bind(&code::RuleDefinition::SetBlock, _val, _1) ]
			>> '}'
	;
}


} // namespace grammar
