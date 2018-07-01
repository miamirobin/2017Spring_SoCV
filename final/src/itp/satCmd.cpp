/****************************************************************************
  FileName     [ satCmd.cpp ]
  PackageName  [ sat ]
  Synopsis     [ Define basic sat prove package commands ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010 LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#include <iomanip>
#include <cstring>
#include "v3NtkUtil.h"
#include "v3StrUtil.h"
#include "v3Msg.h"
#include "satCmd.h"
#include "satMgr.h"

using namespace std;

static SATMgr* satMgr = new SATMgr();

bool
initItpCmd() {
  return  ( v3CmdMgr->regCmd("SATVerify ITP",  4, 3, new SATVerifyItpCmd) &&
            v3CmdMgr->regCmd("SATVerify BMC",  4, 3, new SATVerifyBmcCmd) );
}

static bool valid()
{
  V3NtkHandler* const handler = v3Handler.getCurHandler();

  if (!handler)
    Msg(MSG_WAR) << "Design does not exist !!!" << endl;
  else if (handler->getNtk()->getModuleSize())
    Msg(MSG_WAR) << "Design has not been flattened !!!" << endl;
  else if (dynamic_cast<const V3BvNtk*>(handler->getNtk()))
    Msg(MSG_ERR) << "Current Network is NOT an AIG Ntk (try \"blast ntk\" first)!!" << endl;
  else return true;
  return false;
}

//----------------------------------------------------------------------
//    SATVerify ITP < -Netid <netId> | -Output <outputIndex> > >
//----------------------------------------------------------------------
V3CmdExecStatus
SATVerifyItpCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;

  vector<string> options;
  V3CmdExec::lexOptions(option, options);

  if( options.size() < 2 )
    return V3CmdExec::errorOption(CMD_OPT_MISSING, "");
  if( options.size() > 2)
    return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

  V3NtkHandler* const handler = v3Handler.getCurHandler();
  V3Ntk* const ntk = handler->getNtk();
  bool isNet = false;

  if (v3StrNCmp("-Netid", options[0], 2) == 0) isNet = true;
  else if (v3StrNCmp("-Output", options[0], 2) == 0) isNet = false;
  else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

  int num = 0;
  V3NetId netId;
  if(!v3Str2Int(options[1], num) || (num < 0))
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
  if(isNet) {
    if((unsigned)num >= ntk->getNetSize()) {
      Msg(MSG_ERR) << "Net with Id " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = V3NetId::makeNetId(num);
  } else {
    if((unsigned)num >= ntk->getOutputSize()) {
      Msg(MSG_ERR) << "Output with Index " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = ntk->getOutput(num);
  }

  satMgr->verifyPropertyItp( handler->getNetNameOrFormedWithId(netId), netId);

  return CMD_EXEC_DONE;
}

void
SATVerifyItpCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: SATVerify ITP < -Netid <netId> | -Output <outputIndex> >" << endl;
}

void
SATVerifyItpCmd::help() const
{
  cout << setw(20) << left << "SATVerify ITP:" << "check the monitor by interpolation-based technique" << endl;
}

//----------------------------------------------------------------------
//    SATVerify BMC < -Netid <netId> | -Output <outputIndex> > >
//----------------------------------------------------------------------
V3CmdExecStatus
SATVerifyBmcCmd::exec(const string& option)
{
  if(!valid()) return CMD_EXEC_ERROR;

  vector<string> options;
  V3CmdExec::lexOptions(option, options);

  if( options.size() < 2 )
    return V3CmdExec::errorOption(CMD_OPT_MISSING, "");
  if( options.size() > 2)
    return V3CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);

  V3NtkHandler* const handler = v3Handler.getCurHandler();
  V3Ntk* const ntk = handler->getNtk();
  bool isNet = false;

  if (v3StrNCmp("-Netid", options[0], 2) == 0) isNet = true;
  else if (v3StrNCmp("-Output", options[0], 2) == 0) isNet = false;
  else return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

  int num = 0;
  V3NetId netId;
  if(!v3Str2Int(options[1], num) || (num < 0))
    return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
  if(isNet) {
    if((unsigned)num >= ntk->getNetSize()) {
      Msg(MSG_ERR) << "Net with Id " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = V3NetId::makeNetId(num);
  } else {
    if((unsigned)num >= ntk->getOutputSize()) {
      Msg(MSG_ERR) << "Output with Index " << num << " does NOT Exist in Current Ntk !!" << endl;
      return V3CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }
    netId = ntk->getOutput(num);
  }

  satMgr->verifyPropertyBmc( handler->getNetNameOrFormedWithId(netId), netId);

  return CMD_EXEC_DONE;
}

void
SATVerifyBmcCmd::usage(const bool& verbose) const
{
  Msg(MSG_IFO) << "Usage: SATVerify BMC < -Netid <netId> | -Output <outputIndex> >" << endl;
}

void
SATVerifyBmcCmd::help() const
{
  cout << setw(20) << left << "SATVerify BMC:" << "check the monitor by bounded model checking" << endl;
}

