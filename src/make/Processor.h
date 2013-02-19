/*
 * Copyright 2013, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef HAM_CODE_PROCESSOR_H
#define HAM_CODE_PROCESSOR_H


#include "code/EvaluationContext.h"
#include "data/StringList.h"
#include "data/TargetPool.h"
#include "data/VariableDomain.h"


namespace ham {
namespace make {


using data::StringList;


class Processor {
public:
								Processor();
								~Processor();

			void				SetCompatibility(
									behavior::Compatibility compatibility);
									// resets behavior as well
			void				SetBehavior(behavior::Behavior behavior);

			void				SetOutput(std::ostream& output);
			void				SetErrorOutput(std::ostream& output);

			void				SetTargetsToBuild(const StringList& targets);

			data::VariableDomain& GlobalVariables()
									{ return fGlobalVariables; }
			data::TargetPool&	Targets()
									{ return fTargets; }

			void				ProcessJambase();
			void				BindTargets();
			void				BuildTargets();

private:
			data::VariableDomain fGlobalVariables;
			data::TargetPool	fTargets;
			code::EvaluationContext fEvaluationContext;
			StringList			fTargetsToBuild;
};


}	// namespace make
}	// namespace ham


#endif	// HAM_CODE_PROCESSOR_H
