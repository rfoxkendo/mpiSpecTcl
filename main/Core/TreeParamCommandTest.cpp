// Template for a test suite.
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CTreeParameterCommand.h"
#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include "TreeTestSupport.h"
#include "TCLInterpreter.h"
#include "TCLList.h"
#include "ListVisitor.h"

#include <string>
#include <vector>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class TreeCommandTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeCommandTest);
  CPPUNIT_TEST(UsageTest);
  CPPUNIT_TEST(ListFunc);	// Test list function.
  CPPUNIT_TEST(ListSubCmd);	// Test list subcommand.
  CPPUNIT_TEST(ListUnique);
  CPPUNIT_TEST(SetPropFunc);  	// Set properties via function.
  CPPUNIT_TEST(SetPropSubCmd);	// Set properties via command.
  CPPUNIT_TEST(SetIncFunc);	// Set increment via function.
  CPPUNIT_TEST(SetIncSubCmd);	// Set incrmenet via command dispatch.
  CPPUNIT_TEST(SetChanFunc);	// Set channels via function.
  CPPUNIT_TEST(SetChanSubCmd);  // Set channels via subcommand.
  CPPUNIT_TEST(SetUnitFunc);   	// Set units via function.
  CPPUNIT_TEST(SetUnitSubCmd);	// Set units via subcommand dispatch.
  CPPUNIT_TEST(SetLimitsFunc);	// Set limits via function.
  CPPUNIT_TEST(SetLimitsSubCmd); // Set limits via sub command dispatch.
  CPPUNIT_TEST(CheckFunc);	// Check if an item was modified via function.
  CPPUNIT_TEST(CheckSubCmd);	// Check if an item was modified via dispatch.
  CPPUNIT_TEST(UncheckFunc);	// Reset change flag via function.
  CPPUNIT_TEST(UncheckSubCmd);   // Reset change flag via dispatch.
  CPPUNIT_TEST(Version);	// Test version (just do via command).
  CPPUNIT_TEST_SUITE_END();


private:
  CTreeParameterCommandActual* m_pCommand;
  CTreeParameter*        m_pIndividual;
  CTreeParameterArray*   m_pArray;
  CTCLInterpreter*       m_pInterp;
public:
  void setUp() {
   
    TreeTestSupport::InitTestInterpreter();
    m_pInterp = TreeTestSupport::getInterpreter();
    m_pCommand = new CTreeParameterCommandActual(TreeTestSupport::getInterpreter()); // With default interp.

    m_pIndividual = new CTreeParameter("moe", "cm");
    m_pArray      = new CTreeParameterArray("george", "mm", 10, 0);
  }
  void tearDown() {
    delete m_pIndividual;
    delete m_pArray;
    TreeTestSupport::ClearMap();
    m_pCommand->unregister();
    delete m_pCommand;
    TreeTestSupport::TeardownTestInterpreter();
  }
protected:
  void UsageTest();
  void ListFunc();
  void ListSubCmd();
  void ListUnique();
  void SetPropFunc();
  void SetPropSubCmd();
  void SetIncFunc();
  void SetIncSubCmd();
  void SetChanFunc();
  void SetChanSubCmd();
  void SetUnitFunc();
  void SetUnitSubCmd();
  void SetLimitsFunc();
  void SetLimitsSubCmd();
  void CheckFunc();
  void CheckSubCmd();
  void UncheckFunc();
  void UncheckSubCmd();
  void Version();
private:
  void ListAllCheck(const char* pComment);
  void ListMoeCheck(const char* pComment);
  void ListGeorgeCheck(const char* pComment);
  vector<string> ResultToStrings();
  vector<string> StringListToStrings(string input);
  void ConsistentList(string description, const char* pComment);
  void ConsistentVector(vector<string> list, const char* pComment);
  std::vector<CTCLObject> makeObjv(int argc, const char** argv);
};

std::vector<CTCLObject> 
TreeCommandTest::makeObjv(int argc, const char** argv) {
  std::vector<CTCLObject> result;
  for (int i =0; i < argc; i++) {
    CTCLObject o; o.Bind(*m_pInterp);
    o = std::string(argv[i]);
    result.push_back(o);
  }

  return result;
}

void 
TreeCommandTest::ConsistentList(string description,  const char* pComment)
{
  m_pInterp->setResult("");
  ListVisitor v("*", *m_pInterp);
  vector<string> items = StringListToStrings(description);
  multimap<string, CTreeParameter*>::iterator p = CTreeParameter::find(items[0]);
  ASSERT(p != CTreeParameter::end());
  v.OnMatch(p->second);
  vector<string> visitorresult = ResultToStrings();
  EQMSG(pComment, description, visitorresult[0]);
}

void
TreeCommandTest::ConsistentVector(vector<string> list, const char* pComment)
{
  for (int i =0; i < list.size(); i++) {
    ConsistentList(list[i],  pComment);
  }
}

vector<string>
TreeCommandTest::StringListToStrings(string input)
{
  vector<string> retval;
  CTCLList list(TreeTestSupport::getInterpreter(), input);
  list.Split(retval);
  return retval;
}

vector<string>
TreeCommandTest::ResultToStrings()
{
  string result =  m_pInterp->GetResultString();
  return StringListToStrings(result);
}



// Test utilities.

void
TreeCommandTest::ListAllCheck(const char* pComment)
{
  vector<string> list = ResultToStrings();
 
  // Ensure we have the right number of items:

  EQMSG(pComment, (size_t)11, list.size());

  // Check consistency of each element in the list with what ListVisitor produces.

  ConsistentVector(list,  pComment);

}

void 
TreeCommandTest::ListMoeCheck(const char* pComment)
{
  vector<string> list = ResultToStrings();

  EQMSG(pComment, (size_t)1, list.size());
  ConsistentList(list[0], pComment);

  // The name must be moe:

  vector<string> items = StringListToStrings(list[0]);
  EQMSG(pComment, string("moe"), items[0]);
}

void
TreeCommandTest::ListGeorgeCheck(const char* pComment)
{
  vector<string> list = ResultToStrings();

  EQMSG(pComment, (size_t)10, list.size());

  // Check the consistency of all the elements.

  ConsistentVector(list, pComment);

  // Demand that the element names be of the form george*

  for (int i=0; i < list.size(); i++) {
    vector<string> items = StringListToStrings(list[i]);
    ASSERT(Tcl_StringMatch(items[0].c_str(), "george*"));
  }

}

CPPUNIT_TEST_SUITE_REGISTRATION(TreeCommandTest);

static string sbusage ="Usage:\n\
     treeparameter -list ?pattern?\n\
     treeparameter -listnew\n\
     treeparameter -set name bins low high inc units\n\
     treeparameter -setinc name inc\n\
     treeparameter -setbins name bins\n\
     treeparameter -setunit name units\n\
     treeparameter -setlimits name low high\n\
     treeparameter -check name\n\
     treeparameter -uncheck name\n\
     treeparameter -create  name low high bins units\n\
     treeparameter -listnew\n\
     treeparameter -version";

void TreeCommandTest::UsageTest() 
{
  string usage = m_pCommand->Usage();
  ASSERT(usage == sbusage);
}
// Test the list function... checking itself is factored out as
// we'll do the same tests with dispatch in ListSubCmd().
// - list all of them.. and pass that to ListAllCheck for checking.
// - list only moe..    and pass that to ListMoeCheck for checking.
// - list only george.. and pass that to ListGeorgeCheck for checking.
void
TreeCommandTest::ListFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  m_pCommand->List(*pInterp,  0, NULL);
  ListAllCheck(" Listing all via direct ::List call");

  pInterp->setResult("");

  const char* argv[1] = {"moe"};
  m_pCommand->List(*pInterp,  1, (argv));
  ListMoeCheck("Listing moe via direct ::List call");

  pInterp->setResult("");
  argv[0] = "george*";
  m_pCommand->List(*pInterp, 1, (argv));
  ListGeorgeCheck("Listing george via direct ::ListCall");

}



void
TreeCommandTest::ListSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();

  
  const char* argv[3] = {"treeparameter", "-list", "moe"};
  auto objv = makeObjv(2, argv);
  (*m_pCommand)(*pInterp, objv);
  ListAllCheck("Listing all via dispatched call");

  pInterp->setResult("");
  auto objvMoe = makeObjv(3, argv);
  (*m_pCommand)(*pInterp, objvMoe);
  ListMoeCheck("Listing moe via dispatched call");

  pInterp->setResult("");
  argv[2] = "george*";
  auto objvGeorge = makeObjv(3, argv);
  (*m_pCommand)(*pInterp, objvGeorge);
  ListGeorgeCheck( "Listing george via dispatched call");

}
void
TreeCommandTest::ListUnique() {
    // A tree parameter should only list once even if its defined more than once:
    
    CTreeParameter secondMoe("moe");
    CTreeParameter::BindParameters();
    CTCLInterpreter* interp = TreeTestSupport::getInterpreter();
    const char* argv[3] =  {"treeparameter", "-list", "moe"};
    auto objv = makeObjv(3, argv);
    (*m_pCommand)(*interp, objv);
    ListMoeCheck("Unique check");
    
}
// Set properties of moe via a direct call to the SetDefinition function.
// There are many error cases we must also try:
//   != 6 parameters.
//   Channels not a long.
//   Channels <= 0
//   Start not a float.
//   Stop not a float.
//   Stop < Start
//   Width not a float
//   Width inconsistent with channels, start, stop
//
// And then of course the successful case of setting moe with everything just
// right.
//  Note that the name can be a pattern... By this time we assume that
//  the matching visitor classes are tested and that if we can set moe
//  we'll be able to set any glob pattern.
// When it comes time to testing the command version (SetProcSubCmd()), we assume
// that if we can get dispatched with a good command everything should be golden.
void
TreeCommandTest::SetPropFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  //
  // This is 'all good'
  //
  const char* argv[7] = {"moe", "200", "0.0", "200.0", "1.0", "cm", "junk"};

  const char* saved;			// We'll use this to be able to save/restor args.
  int  status;

  // Bad counts:

  status = m_pCommand->SetDefinition(*pInterp, 1, (argv));
  EQMSG("Too few parameters", TCL_ERROR, status);
  status = m_pCommand->SetDefinition(*pInterp, 7, (argv));
  EQMSG("Too many parameters", TCL_ERROR, status);

  // Channels not a long:

  saved   = argv[1];
  argv[1] = "aaa";
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Real number for channels", TCL_ERROR, status);

  // Channels < 0 is illegal.

  argv[1] = "-200";
  status = m_pCommand->SetDefinition(*pInterp,  6, (argv));
  EQMSG("Negative for chanels", TCL_ERROR, status);
  argv[1] = saved;		// Restore the correct original one.

  // Bad values for start.

  saved = argv[2];
  argv[2] = "abcde";
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Bad start value", TCL_ERROR, status);

  argv[2] = "300.0";		// Above the stop value.
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Start > stop", TCL_ERROR, status);
  argv[2] = saved;

  // Bad value for stop.
  
  saved = argv[3];
  argv[3] = "aaa";                // bad stop value.
  status = m_pCommand->SetDefinition(*pInterp,  6, (argv));
  EQMSG("Bad stop value", TCL_ERROR, status);
  argv[3] = saved;

  // Bad values for width:

  saved = argv[4];
  argv[4] = "aaaaa";
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Bad width", TCL_ERROR, status);

  argv[4] = "10.0";		// insconsistent width.
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Inconsitent width", TCL_ERROR, status);

  // Now do it right:

  argv[4] = saved;
  status = m_pCommand->SetDefinition(*pInterp, 6, (argv));
  EQMSG("Should be ok", TCL_OK, status);


  CheckConstructed(*m_pIndividual, " Set with CTreeParameterCommand::SetDefinition",
		   "moe", 200, 0.0, 200.0, 1.0, "cm", true);

}

void
TreeCommandTest::SetPropSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();

  const char* argv[8] = {"treeparameter", "-set", "moe", 
		   "200", "0.0", "200.0", "1.0", "cm"};
  auto objv = makeObjv(8, argv);
  int status = (*m_pCommand)(*pInterp, objv);
  EQMSG("treeparameter -set", TCL_OK, status);

  CheckConstructed(*m_pIndividual, " Set with CTreeParameterCommand::SetDefinition",
		   "moe", 200, 0.0, 200.0, 1.0, "cm", true);
  
}

//  treeparameter -setinc pattern value
//  Things to check for:
//     Too few parameters.
//     Too many parameters.
//     value must be parsed as real.
//     value must be > 0.
//  If value is legal, then the parameter must change.
//
// We only work with 'moe' assuming by now that the visitor logic 
// is right.
//
void
TreeCommandTest::SetIncFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  const char* argv[3] = {"moe", "2.0", "junk"};
  const char* saved;
  int   status;

  // incorrect argument count:

  status = m_pCommand->SetIncrement(*pInterp, 3, (argv));
  EQMSG("Too many params", TCL_ERROR, status);
  status = m_pCommand->SetIncrement(*pInterp,  1, (argv));
  EQMSG("Too few params", TCL_ERROR, status);

  // Bad values for width:

  saved = argv[1];		// so it can be restored:
  argv[1] = "abcde";
  status = m_pCommand->SetIncrement(*pInterp,  2, (argv));
  EQMSG("Unparsable width", TCL_ERROR, status);
  
  argv[1] = "-5.0";
  status  = m_pCommand->SetIncrement(*pInterp, 2, (argv));
  EQMSG("Width negative", TCL_ERROR, status);

  // Correct functioning:

  argv[1] = saved;
  status  = m_pCommand->SetIncrement(*pInterp, 2, (argv));
  EQMSG("Success", TCL_OK, status);

  CheckConstructed(*m_pIndividual, "Set with setincfunc",
		   "moe", 100, 1.0, 201.0, 2.0, "cm", true);


}
void
TreeCommandTest::SetIncSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  const  char* argv[4] = {"treeparameter", "-setinc", "moe", "2.0"}  ;
  auto objv = makeObjv(4, argv);
  int status = (*m_pCommand)(*pInterp,  objv);
  EQMSG("Success by command", TCL_OK, status);

  CheckConstructed(*m_pIndividual, "Set with setincsubcmd",
		   "moe", 100, 1.0, 201.0, 2.0, "cm", true);
}
//
// treeparameter -setbins pattern value
//
//  Potential problems are:
//    Too many parameters.
//    Too Few parameters.
//    value not an integer.
//    value <=0.
//   Then of course it might work too!.
//
void
TreeCommandTest::SetChanFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const  char* argv[3] = {"moe", "200", "omaki"};
  int   status;
  const char* saved;

  // mess with parameter count:

  status = m_pCommand->SetChannelCount(*pInterp, 1, (argv));
  EQMSG("Tooo few params", TCL_ERROR, status);

  status = m_pCommand->SetChannelCount(*pInterp, 3, (argv));
  EQMSG("Too many parameters", TCL_ERROR, status);

  // Mess with the value of the channel count:

  saved = argv[1];
  argv[1] = "aaaaaa";
  status = m_pCommand->SetChannelCount(*pInterp, 2, (argv));
  EQMSG("Bad value for channel count", TCL_ERROR, status);

  argv[1] = "-100";
  status = m_pCommand->SetChannelCount(*pInterp, 2, (argv));
  EQMSG("Negative width", TCL_ERROR, status);

  // Now get it all right

  argv[1] = saved;
  status = m_pCommand->SetChannelCount(*pInterp, 2, (argv));
  EQMSG("Should work", TCL_OK, status);
  EQMSG("Channelcount: (func)", 200U, m_pIndividual->getBins());
  

}

void 
TreeCommandTest::SetChanSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();

  const char* argv[4] = {"treeparameter", "-setbins", "moe", "200"};
  auto objv = makeObjv(4, argv);  
  int status = (*m_pCommand)(*pInterp, objv);
  EQMSG("-setbins dispatched", TCL_OK, status);
  EQMSG("-setbins result dispatched", 200U, m_pIndividual->getBins());
}
//
// Any string is allowed for units so the only error cases we have are that
// There are too many or too few command parameters.
//
void
TreeCommandTest::SetUnitFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char* argv[3] = { "moe", "inches", "extra"};

  int status = m_pCommand->SetUnit(*pInterp, 1, (argv));
  EQMSG("Too few parameters", TCL_ERROR, status);
  status = m_pCommand->SetUnit(*pInterp,  3, (argv));
  EQMSG("Too many parameters", TCL_ERROR, status);

  status = m_pCommand->SetUnit(*pInterp, 2, (argv));
  EQMSG("-setunit function", TCL_OK, status);
  EQMSG("-setunit function result", string("inches"), m_pIndividual->getUnit());

}


void
TreeCommandTest::SetUnitSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char* argv[4] = {"treeparameter", "-setunit", "moe", "inches"};
  auto objv = makeObjv(4, argv);
  int status = (*m_pCommand)(*pInterp, objv);
  EQMSG("-setunit dispatched", TCL_OK, status);
  EQMSG("-setunit dispatched result", string("inches"), m_pIndividual->getUnit());

}
//
// Things that can go wrong that we test for:
//  - Too many params.
//  - Too few params.
//  - Lower limit is not a real
//  - Upper limit is not a real.
//  - Lower limit > upper limit.
//  Then we also check to be sure the darn thing works when it's supposed to work.
//
void
TreeCommandTest::SetLimitsFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();

  const char* argv[4] = {"moe", "100.0", "200.0", "extra"};
  const  char* saved;
  int   status;


  // Improper parameter count:

  status = m_pCommand->SetLimits(*pInterp,  2, (argv));
  EQMSG("setlimits -too few parameters", TCL_ERROR, status);
  status = m_pCommand->SetLimits(*pInterp,  4, (argv));
  EQMSG("setlimits -too many parameters", TCL_ERROR, status);

  // Lower limit not a real or too big:

  saved = argv[1];
  argv[1] = "aaabbb";
  status = m_pCommand->SetLimits(*pInterp,  3, (argv));
  EQMSG("setlimits - lowlimit not real", TCL_ERROR, status);
  argv[1] = "201.0";
  status = m_pCommand->SetLimits(*pInterp, 3, (argv));
  EQMSG("setlimits - lowlimit > hilimit", TCL_ERROR, status);
  argv[1] = saved;

  // Upper limit not real:

  saved = argv[2];
  argv[2] = "aaaaa";
  status = m_pCommand->SetLimits(*pInterp, 3, (argv));
  EQMSG("setlimits - hilimit not real", TCL_ERROR, status);
  argv[2] = saved;

  // proper functioning:

  status = m_pCommand->SetLimits(*pInterp, 3, (argv));
  EQMSG("setlimits - ok status", TCL_OK, status);
  EQMSG("setlimits lowlimitok", 100.0, m_pIndividual->getStart());
  EQMSG("setlimits hilimitok", 200.0, m_pIndividual->getStop());

}

void
TreeCommandTest::SetLimitsSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  

  const char* argv[5] = {"treeparameter", "-setlimits", "moe", "100.0", "200.0"};
  auto objv = makeObjv(5, argv);
  int status = (*m_pCommand)(*pInterp, objv);
  EQMSG("Status of treeparameter -setlimits dispatch", TCL_OK, status);
  EQMSG("setlimits lowlimitok (dispatch)", 100.0, m_pIndividual->getStart());
  EQMSG("setlimits hilimitok (dispatch)", 200.0, m_pIndividual->getStop());

  
}


//
// Test the -check subcommand.
// Things that can go wrong:
//   Too many parameters
//   Too few parameters
//   Parameter does not exist.
// And of course it could work... returning
// a result of 0 for not modified and 1 for modified.
//
void 
TreeCommandTest::CheckFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char*  argv[2] = {"nosuchparameter", "extra"};	// for initial failure.
  int    status;

  // Test proper reaction to incorrect parameter counts:

  status = m_pCommand->Check(*pInterp, 0, (argv));
  EQMSG("too few params", TCL_ERROR, status);
  status = m_pCommand->Check(*pInterp,  2, (argv));
  EQMSG("tto many params", TCL_ERROR, status);

  // Parameter does not exist:

  status = m_pCommand->Check(*pInterp,  1, (argv));
  EQMSG("no such parameter", TCL_ERROR, status);

  // Now it works, and the result should be 0:

  argv[0] = "moe";
  status = m_pCommand->Check(*pInterp,  1, (argv));
  EQMSG("no change status", TCL_OK, status);
  string answer = pInterp->GetResultString();
  EQMSG("no change value", string("0"), answer);

  // Change moe and see if we get the right thing:

  m_pIndividual->setUnit("arbitrary"); // simplest I can do.
  status = m_pCommand->Check(*pInterp, 1, (argv));
  EQMSG("change status", TCL_OK, status);
  answer = pInterp->GetResultString();
  EQMSG("change value", string("1"), answer);

  
}

// Now via dispatch:

void 
TreeCommandTest::CheckSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char*  argv[3] = {"treeparameter", "-check", "moe"};
  int    status;
  auto objv = makeObjv(3, argv);

  status = (*m_pCommand)(*pInterp, objv);
  EQMSG("Nochange status via dispatch", TCL_OK, status);
  string answer = pInterp->GetResultString();
  EQMSG("NOchange result via dispatch", string("0"), answer);

  objv = makeObjv(3, argv);
  m_pIndividual->setUnit("arbitrary");
  status = (*m_pCommand)(*pInterp, objv);
  EQMSG("change status via dispatch", TCL_OK, status);
  answer = pInterp->GetResultString();
  EQMSG("change result via dispatch", string("1"), answer);

}

//  
//   In removing the changed flag from a parameter
//   several things can go wrong including:
//     Too few parameters
//     Too many parameters
//  We need to check both cases where the flag was and was not
//  changed prior to the call.
//  Once more we work on moe... this time as mo* for the heck of it.
void
TreeCommandTest::UncheckFunc()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char* argv[2] = {"notsuchparameter", "extra"};
  int   status;

  // Verify errors on wrong parameter counts:

  status = m_pCommand->UnCheck(*pInterp, 0, (argv));
  EQMSG("uncheck too few params (func)", TCL_ERROR, status);
  status = m_pCommand->UnCheck(*pInterp, 2, (argv));
  EQMSG("uncheck too many params (func)", TCL_ERROR, status);

  // Now uncheck before it was ever checked:

  argv[0] = "mo*";
  status =m_pCommand->UnCheck(*pInterp, 1, (argv));
  EQMSG("uncheck ok unchanged (func)", TCL_OK, status);
  EQMSG("uncheck ok unchanged value (func)", false,
	m_pIndividual->hasChanged());

  // Cange moe and be sure it gets flipped to unchanged:

  m_pIndividual->setUnit("modified"); // This makes hasChanged() true.
  status =m_pCommand->UnCheck(*pInterp, 1, (argv));
  EQMSG("uncheck ok changed (func)", TCL_OK, status);
  EQMSG("uncheck ok changed value (func)", false,
	m_pIndividual->hasChanged());

  
}

void
TreeCommandTest::UncheckSubCmd()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const char* argv[3]  = {"treeparameter", "-uncheck", "mo*"};
  auto objv = makeObjv(3, argv);
  // Assume the functionlity/parsing has already been checked.

  int status = (*m_pCommand)(*pInterp, objv);
  EQMSG("Uncheck via dispatch", TCL_OK, status);
}
//
// Check the version command:
//    - Errors if too many parameters.
//    - version string is correct.
void
TreeCommandTest::Version()
{
  CTCLInterpreter* pInterp = TreeTestSupport::getInterpreter();
  
  const  char* argv[3]  = {"treeparameter", "-version", "extra*"};
  int   status;
  string answer;

  // Too many params:

  auto objv = makeObjv(3, argv);
  status = (*m_pCommand)(*pInterp, objv);
  EQMSG("version too many params", TCL_ERROR, status);

  // Get the version:

  objv = makeObjv(2, argv);
  status = (*m_pCommand)(*pInterp,  objv);
  EQMSG("version status: ", TCL_OK, status);
  answer = pInterp->GetResultString();
  EQMSG("Version answer", CTreeParameter::TreeParameterVersion,
	answer);

}
