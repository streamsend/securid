#include "ruby.h"
#include "acexport.h"

// module RSA
static VALUE rb_mRSA;

// module RSA::SecurID
static VALUE rb_mRSASecurID;

// class RSA::SecurID::SecurIDError < StandardError
static VALUE rb_eSecurIDError;

// def RSA::SecurID.authenticate(username, passcode)
static VALUE t_authenticate (VALUE self, VALUE username, VALUE passcode)
{
	// the authentication handle representing a single authentication
	// context, i.e. a multi-step authentication attempt
	SDI_HANDLE aceHdl;

	// a string containing the username
	SD_CHAR *userID = StringValuePtr(username);

	// a string containing the passcode
	SD_CHAR *pass   = StringValuePtr(passcode);

	// a hint to the developer about how long to display the next
	// prompt string for the user
	SD_I32  respTimeout;

	// an indicator of the maximum number of bytes of data expected
	// in the next developer-supplied response
	SD_I32  nextRespLen;

	// the size of the developer-supplied storage for the prompt
	// string
	SD_I32  promptStrLen = 512;

	// a developer-supplied character array to be filled in by the
	// API with the string that the caller uses as the next message
	// displayed to the user
	SD_CHAR promptStr[promptStrLen];

	// a flag that is set by the API to indicate whether more data
	// is needed by the authentication context
	SD_BOOL moreData;

	// a flag that guides the developer as to whether the next
	// expected response is echoed to the screen
	SD_BOOL echoFlag;

	// the final authentication status
	SD_I32  authStatus;

	// initialize the authentication library. even though it will only do anything
	// the first time it is called, subsequent calls should still return true if the
	// initialization previously succeeded.
	if (!AceInitialize())
	{
		// the authentication library failed to initialize.
		rb_raise(rb_eSecurIDError, "Failed to initialize authentication library");
	}

	int retVal;

	// start our authentication attempt by first sending the username to
	// the authentication manager.
	retVal = AceStartAuth(&aceHdl, userID, strlen(userID), &moreData, &echoFlag, &respTimeout, &nextRespLen, promptStr, &promptStrLen);

	if (retVal != ACM_OK)
	{
		// the authentication attempt could not be started for some reason.
		rb_raise(rb_eSecurIDError, "Failed to start authentication attempt - Code %d", retVal);
	}

	if (!moreData)
	{
		// the authentication manager should have asked for a passcode
		AceCloseAuth(aceHdl);
		rb_raise(rb_eSecurIDError, "Authentication manager did not ask for a passcode");
	}

	// the authentication manager wants us to prompt the user for more data. because
	// this function is non-interactive, we assume the manager wants the passcode. since
	// we already have it, we'll pass it along without prompting the user.
	retVal = AceContinueAuth(aceHdl, pass, strlen(pass), &moreData, &echoFlag, &respTimeout, &nextRespLen, promptStr, &promptStrLen);

	if (retVal != ACM_OK)
	{
		// the authentication attempt could not be continued for some reason.
		AceCloseAuth(aceHdl);
		rb_raise(rb_eSecurIDError, "Failed to continue authentication attempt - Code %d", retVal);
	}

	if (moreData)
	{
		// either our assumption that the authentication manager wanted the passcode was
		// incorrect, or something else went wrong.
		AceCloseAuth(aceHdl);
		rb_raise(rb_eSecurIDError, "Authentication manager asked for more than a passcode");
	}

	// ask the authentication manager for the status of this authentication attempt.
	retVal = AceGetAuthenticationStatus(aceHdl, &authStatus);

	// finalize this authentication attempt by closing our handle.
	AceCloseAuth(aceHdl);

	if (retVal != ACE_SUCCESS)
	{
		// the authentication status could not be retrieved for some reason.
		rb_raise(rb_eSecurIDError, "Failed to retrieve authentication status - Code %d", retVal);
	}

	// check the status of the authentication attempt and return true or false.
	if (authStatus == ACM_OK)
		return Qtrue;
	else if (authStatus == ACM_ACCESS_DENIED)
		return Qfalse;

	rb_raise(rb_eSecurIDError, "Unexpected authentication status - Code %d", authStatus);
}

void Init_securid ()
{
	// module RSA
	rb_mRSA = rb_define_module("RSA");

	// module RSA::SecurID
	rb_mRSASecurID = rb_define_module_under(rb_mRSA, "SecurID");

	// class RSA::SecurID::SecurIDError < StandardError
	rb_eSecurIDError = rb_define_class_under(rb_mRSASecurID, "SecurIDError", rb_eStandardError);

	// def RSA::SecurID.authenticate(username, passcode)
	rb_define_module_function(rb_mRSASecurID, "authenticate", t_authenticate, 2);
}
