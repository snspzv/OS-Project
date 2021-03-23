#include "sharedInfo.h"
#include<vector>
#include<mutex>
#include <cstring>

std::mutex info_mtx;
std::vector<sharedInfo> info_vect;

int name_in_set(char* check_name, int socket)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);
	int name_found = -1;
	int index = 0;

	for (auto& info : info_vect)
	{	
		//check if any names match AND that name is not being compared to its own sharedInfo
		if ((strcmp(check_name, info.name) == 0) && (info.sock_fd != socket))
		{
			name_found = index;
			break;
		}

		index++;
	}

	return name_found;
}

void add_to_set(char* name_in, int vect_index, int socket)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);
	sharedInfo userInfo;

	//create enough sharedInfos until vector is big enough for vect_index to access
	while (info_vect.size() < vect_index + 1)
	{
		info_vect.push_back(userInfo);
	}

	strncpy(info_vect[vect_index].name, name_in, strlen(name_in));

	info_vect[vect_index].sock_fd = socket;

	info_vect[vect_index].busy = false;

	info_vect[vect_index].connection_requested = vect_index;

	return;
}

bool request_partner(int requester_index, int partner_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	bool request_set = false;

	//Alert potential partner requester is requesting
	if (info_vect[partner_index].busy == false)
	{
		request_set = true;
		info_vect[partner_index].connection_requested = requester_index;
		info_vect[partner_index].busy = true;
		info_vect[requester_index].busy = true;
	}

	return request_set;

}

int check_request_status(int requester_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	return info_vect[requester_index].connection_requested;
}

void failed_request_reset(int requester_index, int partner_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[requester_index].connection_requested = requester_index;
	info_vect[partner_index].connection_requested = partner_index;
	return;
}

void set_not_busy(int vect_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[vect_index].busy = false;
	return;
}

bool is_not_busy(int vect_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	return !info_vect[vect_index].busy;
}

int get_requester_name(int vect_index, char* requester_name)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);
	
	memset(requester_name, 0, sizeof requester_name);

	int requester_index = info_vect[vect_index].connection_requested;
	strncpy(requester_name, info_vect[requester_index].name, strlen(info_vect[requester_index].name));
	return requester_index;

}

void accept_request(int vect_index, int requester_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[requester_index].connection_requested = vect_index;
	return;
}

void deny_request(int requester_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[requester_index].connection_requested = -1;
	return;
}

void timeout_request(int requester_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[requester_index].connection_requested = -2;
	return;
}

void set_connection(int vect_index, int partner_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);

	info_vect[vect_index].connection_index = partner_index;
	info_vect[partner_index].connection_index = vect_index;
	
	return;
}

int get_partner_sock(int vect_index)
{
	///will be released when out of scope
	std::lock_guard<std::mutex> lock(info_mtx);
	
	int partner_index = info_vect[vect_index].connection_index;
	return info_vect[partner_index].sock_fd;
}

bool connection_active(int vect_index)
{
	if ((info_vect[vect_index].connection_index) == -1)
		return false;

	return true;
}



