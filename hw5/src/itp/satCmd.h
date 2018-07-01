/****************************************************************************
  FileName     [ satCmd.h ]
  PackageName  [ sat ]
  Synopsis     [ Define basic sat prove package commands ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_CMD_H_
#define SAT_CMD_H_

#include "v3CmdMgr.h"

// ============================================================================
// Classes for Prove package commands 
// ============================================================================
V3_COMMAND(SATVerifyItpCmd,  CMD_TYPE_ITP);
V3_COMMAND(SATVerifyBmcCmd,  CMD_TYPE_ITP);

#endif

