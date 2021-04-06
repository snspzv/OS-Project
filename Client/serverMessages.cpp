#include <map>
#include "Constants.h"
#include <string>

std::map<int, std::string> servMessages = {
	{USER_MESSAGE, ""},
	{UNAME_REQUEST, "Enter your username: "},
	{UNAME_TAKEN, "The username you entered is already taken. Try again."},
	{GET_PARTNER, "Wait for a connection or enter the name of the user you'd like to message: "},
	{UNAME_VALID, "Valid username!"},
	{PARTNER_HAS_REQUESTED, " has requested to message with you. Do you want to message them?(y/n) "},
	{PARTNER_DNE, "Invalid name. Try again"},
	{CONN_MADE, "Connection Successful."},
	{CONN_DENIED, "Connection Denied."},
	{NOT_AVAIL, "This user is currently not available :("},
	{TIMEOUT, "Your request has timed out"},
	{BAD_RESPONSE, "Invalid response. Please answer 'y' or 'n'"},
	{PARTNER_FOUND, " found, asking them now"},
	{PARTNER_DISCONNECT, " disconnected. Ending session..."}
};

