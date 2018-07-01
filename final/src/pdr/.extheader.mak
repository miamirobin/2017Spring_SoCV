pdr.d: ../../include/SolverV.h ../../include/ProofV.h ../../include/SolverTypesV.h ../../include/VarOrderV.h ../../include/FileV.h ../../include/GlobalV.h ../../include/HeapV.h ../../include/SortV.h 
../../include/SolverV.h: SolverV.h
	@rm -f ../../include/SolverV.h
	@ln -fs ../src/pdr/SolverV.h ../../include/SolverV.h
../../include/ProofV.h: ProofV.h
	@rm -f ../../include/ProofV.h
	@ln -fs ../src/pdr/ProofV.h ../../include/ProofV.h
../../include/SolverTypesV.h: SolverTypesV.h
	@rm -f ../../include/SolverTypesV.h
	@ln -fs ../src/pdr/SolverTypesV.h ../../include/SolverTypesV.h
../../include/VarOrderV.h: VarOrderV.h
	@rm -f ../../include/VarOrderV.h
	@ln -fs ../src/pdr/VarOrderV.h ../../include/VarOrderV.h
../../include/FileV.h: FileV.h
	@rm -f ../../include/FileV.h
	@ln -fs ../src/pdr/FileV.h ../../include/FileV.h
../../include/GlobalV.h: GlobalV.h
	@rm -f ../../include/GlobalV.h
	@ln -fs ../src/pdr/GlobalV.h ../../include/GlobalV.h
../../include/HeapV.h: HeapV.h
	@rm -f ../../include/HeapV.h
	@ln -fs ../src/pdr/HeapV.h ../../include/HeapV.h
../../include/SortV.h: SortV.h
	@rm -f ../../include/SortV.h
	@ln -fs ../src/pdr/SortV.h ../../include/SortV.h
