/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** Unit tests for the
 ** spectcl_experiment.c module
 */
#include <check.h>
#include "spectcl_experiment.h"
#include <sqlite3.h>
#include "spectcl_experimentInternal.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <uuid/uuid.h>
#include <string.h>
#include <limits.h>



static char dbName[PATH_MAX];
static char wsName[PATH_MAX];
static char wsName1[PATH_MAX];


spectcl_experiment db;
spectcl_workspace  ws;

#ifndef FALSE
#define FALSE 0
#endif


#ifndef TRUE
#define TRUE 1
#endif
/*----------------------------------- fixture code ----------------------------------------*/

static void setup()
{
  strcpy(dbName, tmpnam(NULL));
  db     = spectcl_experiment_create(dbName);
  strcpy(wsName, tmpnam(NULL));
  spectcl_workspace_create(db, wsName);
  ws = spectcl_workspace_open(wsName);
}

static void teardown()
{
  spectcl_workspace_close(ws);
  spectcl_experiment_close(db);
  unlink(dbName);
  unlink(wsName);
}
/*----------------- test cases ---------------*/

/**
 ** ensure the correct set of schema were made
 ** for spectrum definitions when a workspace is created.
 ** Specifically we should be able to make a query from
 ** spectrum_definitions and
 ** spectrum_parameters
 ** (with zero rows coming back).
 **
 ** 
 */
START_TEST(test_schema)
{
  sqlite3_stmt* stmt;
  const char* query1 = "SELECT * FROM spectrum_definitions";
  const char* query2 = "SELECT * FROM spectrum_parameters";
  int   status;

  status = sqlite3_prepare_v2(ws, query1,
			      -1, &stmt, NULL);
  fail_unless(status == SQLITE_OK);
  status = sqlite3_step(stmt);
  fail_unless(status == SQLITE_DONE);
  sqlite3_finalize(stmt);
  
  status = sqlite3_prepare_v2(ws, query2,
			      -1, &stmt, NULL);
  fail_unless(status == SQLITE_OK);
  status = sqlite3_step(stmt);
  fail_unless(status == SQLITE_DONE);
  sqlite3_finalize(stmt);
}
END_TEST

/**
 ** Experiment databases must be experiment
 ** databases for the create primitive:
 */
START_TEST(test_create_notexp)
{
  int status = spectcl_workspace_create_spectrum(ws,
						  NULL,
						  NULL,
						  NULL,
						  NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_EXPDATABASE);
					     
}
END_TEST
/**
 ** There must be an attached workspace for a spectrum
 ** to be created in it:
 */
START_TEST(test_create_notattached)
{
  int status = spectcl_workspace_create_spectrum(db,
						  NULL,
						  NULL,
						  NULL,
						  NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_UNATTACHED);

}
END_TEST

/*
  Ensure spectrum type must be valid:
*/
START_TEST(test_create_badtype)
{
  int status;
  spectcl_workspace_attach(db, wsName, NULL);
  status = spectcl_workspace_create_spectrum(db,
					      "XYZZY",
					      NULL, 
					      NULL,
					      NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_INVTYPE);

}
END_TEST
/*
** Parameter must be ok.
*/
START_TEST(test_create_badparam)
{
  spectrum_parameter p1 = {
    "param1",
    1
  };
  spectrum_parameter* params[2]  = {
    &p1, NULL
  };

  int status;

  spectcl_workspace_attach(db, wsName, NULL);
  status = spectcl_workspace_create_spectrum(db,
					      "1",
					      "spectrum.test",
					      params,
					      NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_NOSUCH);
  
}
END_TEST
/*
** 1-d spectrum - only dimension 1 is allowed
*/
START_TEST(test_create_1dbaddim)
{
  spectrum_parameter p1 = {
    "param1",
    2				/* Illegal dim number for 1d. */
  };
  spectrum_parameter* params[2]  = {
    &p1, NULL
  };

  int status;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);

  status = spectcl_workspace_create_spectrum(db,
					     "1", "spectrum.test",
					     params, NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_BADPARAMS);

}
END_TEST

/**
 * 1-d spectrum - only one parameter is allowed:
 */
START_TEST(test_create_1dbadcount)
{
  spectrum_parameter p1 = {
    "param1",
    1				/* Illegal dim number for 1d. */
  };
  spectrum_parameter p2 = {
    "param2",
    1
  };
  spectrum_parameter* params[3] = {
    &p1, &p2, NULL
  };
  int status;


  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);
  spectcl_parameter_create(db, "param2", "arb", NULL, NULL);

  status = spectcl_workspace_create_spectrum(db,
					     "1", "spectrum.test",
					     params, NULL);
  fail_unless(status == -1);
  fail_unless(spectcl_experiment_errno == SPEXP_BADPARAMS);
}
END_TEST

/*
** Test that the first creation of a spectrum produces the correct results:
** - return value > 0, and SPEXP_OK
*/
START_TEST(test_create_1dgood1st)
{
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  int status;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);

   status = spectcl_workspace_create_spectrum(db,
					     "1", "spectrum.test",
					     params, NULL);
   fail_if(status <= 0);
   fail_unless(spectcl_experiment_errno == SPEXP_OK);
}
END_TEST

/**
 ** If I create a new spectrum that already exists I should get a new version
 */
START_TEST(test_create_1dgoodnewvers)
{
  spectrum_parameter p1 = {
    "param1",
    1				/* Illegal dim number for 1d. */
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  int status;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);

  spectcl_workspace_create_spectrum(db,
				   "1", "spectrum.test",
				   params, NULL);
  status = spectcl_workspace_create_spectrum(db,
					    "1", "spectrum.test",
					    params, NULL);
  fail_unless(status == 2);	/* second spectrum id is 2. */
  fail_unless(spectcl_experiment_errno == SPEXP_NEWVERS);
}
END_TEST

/*------------------ Test find_spectra ------*/

/* Must pass an experiment database: */

START_TEST(test_find_notexp)
{
  spectrum_definition** ppDefs = spectcl_workspace_find_spectra(ws,
								NULL,
								TRUE,
								NULL);
  fail_unless(ppDefs == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_NOT_EXPDATABASE);
}
END_TEST

/* Experiment database must have a workspace attached: */

START_TEST(test_find_notattached)
{
  spectrum_definition** ppDefs = spectcl_workspace_find_spectra(db,
								NULL,
								TRUE,
								NULL);
  fail_unless(ppDefs == NULL);
  fail_unless(spectcl_experiment_errno == SPEXP_UNATTACHED);
 
							       
}
END_TEST
/*  Finding with no spectra defined should give an empty result list. */

START_TEST(test_find_emptyresult)
{
  spectrum_definition** ppDefs;
  spectcl_workspace_attach(db, wsName, NULL); /* Default location is fine. */
  ppDefs = spectcl_workspace_find_spectra(db,
					  NULL,
					  TRUE,
					  NULL);
  fail_if(ppDefs == NULL);
  if (ppDefs) {
    fail_unless(ppDefs[0] == NULL);
    free(ppDefs);		/* That's enough to free stuff. */
  }

}
END_TEST
/*  Finding when a single spectrum is defined. */

START_TEST(test_find_oneresult)
{
  spectrum_definition** ppDefs;
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  int status;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);

  status = spectcl_workspace_create_spectrum(db,
					     "1", "spectrum.test",
					     params, NULL);
  status= spectcl_experiment_errno;
  fail_unless(status == SPEXP_OK);
  if (status != SPEXP_OK) return;

  ppDefs = spectcl_workspace_find_spectra(db,
					  NULL, TRUE,
					  NULL);
  fail_if(ppDefs == NULL);

  if (ppDefs) {
    spectrum_definition* pDef = ppDefs[0];
    
    fail_if(ppDefs[0] == NULL);
    if(ppDefs[0]) {
      fail_if(ppDefs[1] != NULL);
      
      fail_unless(pDef->s_id == 1); 
      fail_unless(strcmp(pDef->s_name, "spectrum.test") == 0);
      fail_unless(strcmp(pDef->s_type, "1") == 0);
      fail_unless(pDef->s_version == 1);
      fail_if(pDef->s_parameters == NULL);
      if(pDef->s_parameters) {
	spectrum_parameter* pParam = pDef->s_parameters[0];
	
	fail_unless(strcmp(pParam->s_name, "param1") == 0);
	fail_unless(pParam->s_dimension == 1);
	
	free(pParam->s_name);
	free(pParam);
      }
      free(pDef->s_name);
      free(pDef->s_type);
      free(pDef);
    }




    free(ppDefs);
  }
}
END_TEST
/*
 * Defining several spectra should allow me to see both of them.
 * Since the spectra are ordered by id I shouild get them in definition order.
 */
START_TEST(test_find_multiple) 
{
  spectrum_definition** ppDefs;
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter p2 = {
    "param2",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  spectrum_parameter* pspec2[2] = {
    &p2, NULL
  };
  const char* specNames[2] = {"spectrum.test", "another_Spectrum"};
  const char* parNames[2]  = {"param1", "param2"};
  int status;
  int i;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);
  spectcl_parameter_create(db, "param2", "stuff", NULL, NULL);

  spectcl_workspace_create_spectrum(db,
				    "1", specNames[0],
				    params, NULL);
  spectcl_workspace_create_spectrum(db, "1", specNames[1],
				    pspec2, NULL);

  ppDefs = spectcl_workspace_find_spectra(db,
					  NULL, TRUE,
					  NULL);
  fail_if(ppDefs == NULL);

  if (ppDefs) {			/* Should be two defs. */
    for ( i = 0; i < 2; i++) {
      spectrum_definition* pDef = ppDefs[i];
      if (!pDef) {
	fail();
	return;
      }
      fail_unless(pDef->s_id == (i+1));
      fail_unless(strcmp(pDef->s_name, specNames[i]) == 0);
      fail_unless(strcmp(pDef->s_type, "1") == 0);
      fail_unless(pDef->s_version == 1); /* both unique so version 1. */
      fail_if(pDef->s_parameters == NULL);
      if (pDef->s_parameters) {
	spectrum_parameter* pParam = pDef->s_parameters[0];
	fail_unless(strcmp(pParam->s_name, parNames[i]) == 0);
	fail_unless(pParam->s_dimension == 1);

	free(pParam->s_name);
	free(pParam);

      }
      free(pDef->s_name);
      free(pDef->s_type);
      free(pDef);
      
    }
  }
  fail_unless(ppDefs[2] == NULL);
  free(ppDefs);



}
END_TEST
/*
 *  If there are several versions of a spectrum I should
 *  be able to get the most recent one if that's what I ask
 *  for
 */
START_TEST(test_find_mostrecent)
{
  spectrum_definition** ppDefs;
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter p2 = {
    "param2",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  spectrum_parameter* pspec2[2] = {
    &p2, NULL
  };
  int status;
  int i;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);
  spectcl_parameter_create(db, "param2", "stuff", NULL, NULL);

  spectcl_workspace_create_spectrum(db,
				    "1", "spectrum.test",
				    params, NULL);
  spectcl_workspace_create_spectrum(db, "1", "spectrum.test",
				    pspec2, NULL); /* version 2. */

  ppDefs = spectcl_workspace_find_spectra(db,
					  NULL, FALSE,
					  NULL);
  fail_if(ppDefs == NULL);
  if (ppDefs) {
    spectrum_definition* pDef = ppDefs[0];
    
    fail_if(ppDefs[0] == NULL);
    if(ppDefs[0]) {
      fail_if(ppDefs[1] != NULL);
      
      fail_unless(pDef->s_id == 2); 
      fail_unless(strcmp(pDef->s_name, "spectrum.test") == 0);
      fail_unless(strcmp(pDef->s_type, "1") == 0);
      fail_unless(pDef->s_version == 2);
      fail_if(pDef->s_parameters == NULL);
      if(pDef->s_parameters) {
	spectrum_parameter* pParam = pDef->s_parameters[0];
	
	fail_unless(strcmp(pParam->s_name, "param2") == 0);
	fail_unless(pParam->s_dimension == 1);
	
	free(pParam->s_name);
	free(pParam);
      }
      free(pDef->s_name);
      free(pDef->s_type);
      free(pDef);
    }




    free(ppDefs);
  }
  
}
END_TEST

/*
 * Should be able to ask for all versions of a spectrum and get it
 * in low to high version order
 */
START_TEST(test_find_multiversion)
{
  spectrum_definition** ppDefs;
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter p2 = {
    "param2",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  spectrum_parameter* pspec2[2] = {
    &p2, NULL
  };
  int status;
  int i;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);
  spectcl_parameter_create(db, "param2", "stuff", NULL, NULL);

  spectcl_workspace_create_spectrum(db,
				    "1", "spectrum.test",
				    params, NULL);
  spectcl_workspace_create_spectrum(db, "1", "spectrum.test",
				    pspec2, NULL); /* version 2. */

  ppDefs = spectcl_workspace_find_spectra(db,
					  NULL, TRUE,
					  NULL);
  fail_if(ppDefs == NULL);
  if (ppDefs) {
    fail_if(ppDefs[0] == NULL);
    if (ppDefs[0]) {		/* TODO: free memory. */
      spectrum_definition* pDef = ppDefs[0];
      fail_unless(pDef->s_version == 2);
      fail_if(ppDefs[1] == NULL);
      if (ppDefs[1]) {
	pDef = ppDefs[1];
	fail_unless(pDef->s_version == 1);
	fail_unless(ppDefs[2] == NULL);
      }
    }

  }
  
}
END_TEST

/*
 * should be able to use patterns to search only for specific spectra.
 */
START_TEST(test_find_specific)
{
  spectrum_definition** ppDefs;
  spectrum_parameter p1 = {
    "param1",
    1				
  };
  spectrum_parameter p2 = {
    "param2",
    1				
  };
  spectrum_parameter* params[3] = {
    &p1, NULL
  };
  spectrum_parameter* pspec2[2] = {
    &p2, NULL
  };
  const char* specNames[2] = {"spectrum.test", "another_Spectrum"};
  const char* parNames[2]  = {"param1", "param2"};
  int status;
  int i;

  spectcl_workspace_attach(db, wsName, NULL);
  spectcl_parameter_create(db, "param1", "arb", NULL, NULL);
  spectcl_parameter_create(db, "param2", "stuff", NULL, NULL);

  spectcl_workspace_create_spectrum(db,
				    "1", specNames[0],
				    params, NULL);
  spectcl_workspace_create_spectrum(db, "1", specNames[1],
				    pspec2, NULL);
  ppDefs = spectcl_workspace_find_spectra(db,
					  "another*",
					  TRUE, NULL);

  /* Should only get another_Spectrum back */

  fail_if(ppDefs == NULL);
  if (ppDefs) {
    /* TODO: Memory mangement */

    fail_if(ppDefs[0] == NULL);
    if (ppDefs[0]) {
      /* By now I'm confident the right data are hooked up with the right'
	 spectra so just check that the name is correct */
     
      fail_unless(strcmp(ppDefs[0]->s_name, "another_Spectrum") == 0);
    }
  }
}
END_TEST

/*------------------- Final setup  -----------*/
int main(void) 
{
  int  failures;

  Suite* s = suite_create("spectcl_workspace_spectra");
  SRunner* sr = srunner_create(s);
  TCase*   tc_spectra = tcase_create("spectra");

  tcase_add_checked_fixture(tc_spectra, setup, teardown);
  suite_add_tcase(s, tc_spectra);

  /* Schema test(s) */

  tcase_add_test(tc_spectra, test_schema);

  /* Spectrum creation tests */

  tcase_add_test(tc_spectra, test_create_notexp);
  tcase_add_test(tc_spectra, test_create_notattached);
  tcase_add_test(tc_spectra, test_create_badtype);
  tcase_add_test(tc_spectra, test_create_badparam);
  tcase_add_test(tc_spectra, test_create_1dbaddim);
  tcase_add_test(tc_spectra, test_create_1dbadcount);
  tcase_add_test(tc_spectra, test_create_1dgood1st);
  tcase_add_test(tc_spectra, test_create_1dgoodnewvers);

  /* Spectrum listing tests   */

  tcase_add_test(tc_spectra, test_find_notexp);
  tcase_add_test(tc_spectra, test_find_notattached);
  tcase_add_test(tc_spectra, test_find_emptyresult);
  tcase_add_test(tc_spectra, test_find_oneresult);
  tcase_add_test(tc_spectra, test_find_multiple);
  tcase_add_test(tc_spectra, test_find_mostrecent);
  tcase_add_test(tc_spectra, test_find_multiversion);
  tcase_add_test(tc_spectra, test_find_specific);


  srunner_set_fork_status(sr, CK_NOFORK);


  srunner_run_all(sr, CK_NORMAL);
  failures = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
