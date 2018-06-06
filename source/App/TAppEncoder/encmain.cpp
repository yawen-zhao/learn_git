/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2015, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     encmain.cpp
    \brief    Encoder application main
*/

#include <time.h>
#include <iostream>
#include "TAppEncTop.h"
#include "TAppCommon/program_options_lite.h"

//! \ingroup TAppEncoder
//! \{

#include "../Lib/TLibCommon/Debug.h"

// ====================================================================================================================
// Main function
// ====================================================================================================================

int main(int argc, char* argv[])
{
  TAppEncTop  cTAppEncTop;

  // print information
  fprintf( stdout, "\n" );
#if NH_MV
  fprintf( stdout, "3D-HTM Software: Encoder Version [%s] based on HM Version [%s]", NV_VERSION, HM_VERSION );  
#else
  fprintf( stdout, "HM software: Encoder Version [%s] (including RExt)", NV_VERSION );
#endif
  fprintf( stdout, NVM_ONOS );
  fprintf( stdout, NVM_COMPILEDBY );
  fprintf( stdout, NVM_BITS );
  fprintf( stdout, "\n\n" );

#if OUTPUT_DMM_NUM
  ::memset(DMM_num_original, 0, sizeof(long) * 4);
  ::memset(DMM_num, 0, sizeof(long) * 4);
  ::memset(DMM_num_OB1, 0, sizeof(long) * 4);
  ::memset(DMM_num_finally, 0, sizeof(long) * 4);
  DMM_num_1 = 0;
#endif

#if LCU_ANALYSIS
  ::memset(g_iLCUDepthForCPandNCP, 0, sizeof(int) * 32);
#endif

#if DMM_TIME
  TimeForTotalDepth = 0;
  TimeForEachFrame = 0;
#endif

#if CORNER_POINT
  ::memset(g_iNumberCornerPoint, 0, sizeof(int)* 6);  // [layer]
  ::memset(g_ChangFlag, 0, sizeof(bool)* 6);
  g_qp = 0;
#endif

  // create application encoder class
  cTAppEncTop.create();

  // parse configuration
  try
  {
    if(!cTAppEncTop.parseCfg( argc, argv ))
    {
      cTAppEncTop.destroy();
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
      EnvVar::printEnvVar();
#endif
      return 1;
    }
  }
  catch (df::program_options_lite::ParseFailure &e)
  {
    std::cerr << "Error parsing option \""<< e.arg <<"\" with argument \""<< e.val <<"\"." << std::endl;
    return 1;
  }

#if PRINT_MACRO_VALUES
  printMacroSettings();
#endif

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
  EnvVar::printEnvVarInUse();
#endif

  // starting time
  Double dResult;
  clock_t lBefore = clock();

  // call encoding function
  cTAppEncTop.encode();

  // ending time
  dResult = (Double)(clock()-lBefore) / CLOCKS_PER_SEC;
  printf("\n Total Time: %12.3f sec.\n", dResult);

#if LCU_ANALYSIS
  ofstream outputCUAnalysis;
  outputCUAnalysis.open("LCUAnalysis.xls", ios::app | ios::out);
#endif

#if LCU_ANALYSIS
  //*************************************LCU Depth Level**********************************
  for (int cor = 0; cor<2; cor++)
  {
	  for (int iPre = 0; iPre<4; iPre++)
	  {
		  for (int idep = 0; idep<4; idep++)
		  {
			  outputCUAnalysis << g_iLCUDepthForCPandNCP[cor][iPre][idep] << "\t";
		  }
		  outputCUAnalysis << "\n";
	  }
	  outputCUAnalysis << "\n";
  }
  outputCUAnalysis.flush();
  outputCUAnalysis.close();
  //**************************************************************************************************
#endif

#if OUTPUT_DMM_NUM
  cout << "DMM_num_1: " << DMM_num_1<<endl;

  cout << "DMM_num_original: " <<'\t';
  for (int i = 0; i < 4; i++)
	  cout << DMM_num_original[i] << '\t';
  cout << endl;

  cout << "DMM_num : " <<  '\t';
  for (int i = 0; i < 4; i++)
	  cout << DMM_num[i] << '\t';
  cout << endl;

  cout << "DMM_num_OB1: " << '\t';
  for (int i = 0; i < 4; i++)
	  cout << DMM_num_OB1[i] << '\t';
  cout << endl;

  cout << "DMM_num_finally: " << '\t';
  for (int i = 0; i < 4; i++)
	  cout << DMM_num_finally[i] << '\t';
  cout << endl;
#endif

#if DMM_TIME
  cout << "total time for DMM: " << TimeForTotalDepth << endl;
#endif
  // destroy application encoder class
  cTAppEncTop.destroy();

  return 0;
}

//! \}
