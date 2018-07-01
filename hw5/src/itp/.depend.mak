reader.o: reader.cpp reader.h
satMgr.o: satMgr.cpp ../../include/v3Msg.h ../../include/v3NtkUtil.h \
 ../../include/v3NtkHandler.h ../../include/v3Ntk.h \
 ../../include/v3Misc.h ../../include/v3Map.h ../../include/v3Set.h \
 ../../include/v3Vec.h ../../include/v3List.h ../../include/v3Hash.h \
 ../../include/v3HashUtil.h ../../include/v3Type.h \
 ../../include/v3IntType.h ../../include/v3Queue.h \
 ../../include/v3Stack.h ../../include/v3BitVec.h reader.h satMgr.h \
 ../../include/v3Ntk.h sat.h SolverV.h SolverTypesV.h GlobalV.h \
 VarOrderV.h HeapV.h ProofV.h FileV.h ../../include/bddNodeV.h \
 ../../include/bddMgrV.h ../../include/myHash.h ../../include/bddNodeV.h
ProofV.o: ProofV.cpp ProofV.h SolverTypesV.h GlobalV.h FileV.h SortV.h
sat.o: sat.cpp sat.h ../../include/v3Ntk.h ../../include/v3Misc.h \
 ../../include/v3Map.h ../../include/v3Set.h ../../include/v3Vec.h \
 ../../include/v3List.h ../../include/v3Hash.h ../../include/v3HashUtil.h \
 ../../include/v3Type.h ../../include/v3IntType.h ../../include/v3Queue.h \
 ../../include/v3Stack.h ../../include/v3BitVec.h SolverV.h \
 SolverTypesV.h GlobalV.h VarOrderV.h HeapV.h ProofV.h FileV.h
satCmd.o: satCmd.cpp ../../include/v3NtkUtil.h \
 ../../include/v3NtkHandler.h ../../include/v3Ntk.h \
 ../../include/v3Misc.h ../../include/v3Map.h ../../include/v3Set.h \
 ../../include/v3Vec.h ../../include/v3List.h ../../include/v3Hash.h \
 ../../include/v3HashUtil.h ../../include/v3Type.h \
 ../../include/v3IntType.h ../../include/v3Queue.h \
 ../../include/v3Stack.h ../../include/v3BitVec.h \
 ../../include/v3StrUtil.h ../../include/v3Msg.h satCmd.h \
 ../../include/v3CmdMgr.h satMgr.h ../../include/v3Ntk.h sat.h SolverV.h \
 SolverTypesV.h GlobalV.h VarOrderV.h HeapV.h ProofV.h FileV.h reader.h
SolverV.o: SolverV.cpp SolverV.h SolverTypesV.h GlobalV.h VarOrderV.h \
 HeapV.h ProofV.h FileV.h SortV.h
FileV.o: FileV.cpp FileV.h GlobalV.h
